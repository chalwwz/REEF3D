/*--------------------------------------------------------------------
REEF3D
Copyright 2008-2019 Hans Bihs

This file is part of REEF3D.

REEF3D is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, see <http://www.gnu.org/licenses/>.
--------------------------------------------------------------------
--------------------------------------------------------------------*/

#include"sflow_turb_kw_IM1.h"
#include"lexer.h"
#include"fdm2D.h"
#include"ghostcell.h"
#include"sflow_idiff.h"
#include"solver2D.h"
#include"sflow_iweno_hj.h"

#define HP (fabs(b->hp(i,j))>1.0e-20?b->hp(i,j):1.0e20)

sflow_turb_kw_IM1::sflow_turb_kw_IM1(lexer* p) : kin(p), omega(p), kn(p), wn(p), Pk(p), S(p), ustar(p), cf(p),
                                                 kw_alpha(5.0/9.0), kw_beta(3.0/40.0),kw_sigma_k(2.0),kw_sigma_w(2.0)
{
    gcval_kin=20;
	gcval_omega=30;
    
    pconvec = new sflow_iweno_hj(p);
    pdiff = new sflow_idiff(p);
}

sflow_turb_kw_IM1::~sflow_turb_kw_IM1()
{
}

void sflow_turb_kw_IM1::start(lexer *p, fdm2D *b, ghostcell *pgc, sflow_convection *pdisc, sflow_diffusion *pdiffmom, solver2D *psolv, ioflow *pflow)
{
    Pk_update(p,b,pgc);
    ustar_update(p,b,pgc);

//kin
    starttime=pgc->timer();
	clearrhs(p,b);
    pconvec->start(p,b,kin,4,b->P,b->Q);
    pdiff->diff_scalar(p,b,pgc,psolv,kin,kw_sigma_k,1.0);
	kin_source(p,b);
	timesource(p,b,kn);
    psolv->start(p,pgc,kin,b->M,b->xvec,b->rhsvec,4,gcval_kin,p->T13,b->C4);
    pgc->gcsl_start4(p,kin,gcval_kin);
	p->kintime=pgc->timer()-starttime;
	p->kiniter=p->solveriter;
	if(p->mpirank==0 && (p->count%p->P12==0))
	cout<<"kin_iter: "<<p->kiniter<<"  kin_time: "<<setprecision(3)<<p->kintime<<endl;

//omega
    starttime=pgc->timer();
	clearrhs(p,b);
    pconvec->start(p,b,omega,4,b->P,b->Q);
    pdiff->diff_scalar(p,b,pgc,psolv,omega,kw_sigma_w,1.0);
	omega_source(p,b);
	timesource(p,b,wn);
	psolv->start(p,pgc,omega,b->M,b->xvec,b->rhsvec,4,gcval_omega,p->T13,b->C4);
    pgc->gcsl_start4(p,omega,gcval_omega);
	p->epstime=pgc->timer()-starttime;
	p->epsiter=p->solveriter;
	if(p->mpirank==0 && (p->count%p->P12==0))
	cout<<"omega_iter: "<<p->epsiter<<"  omega_time: "<<setprecision(3)<<p->epstime<<endl;

	eddyvisc(p,b,pgc);
}

void sflow_turb_kw_IM1::ktimesave(lexer* p, fdm2D *b, ghostcell *pgc)
{
    SLICELOOP4
    kn(i,j) = kin(i,j);
}

void sflow_turb_kw_IM1::etimesave(lexer* p, fdm2D *b, ghostcell *pgc)
{
    SLICELOOP4
    wn(i,j) = omega(i,j);
}
    
void sflow_turb_kw_IM1::eddyvisc(lexer* p, fdm2D *b, ghostcell *pgc)
{
    SLICELOOP4
    b->eddyv(i,j) = MAX(MIN(MAX(kin(i,j)
                        /((omega(i,j))>(1.0e-20)?(omega(i,j)):(1.0e20)),0.0),fabs(p->T31*kin(i,j))/S(i,j)),
                        0.0001*p->W2);

	pgc->gcsl_start4(p,b->eddyv,24);
}

void sflow_turb_kw_IM1::kin_source(lexer* p, fdm2D *b)
{
    count=0;
    SLICELOOP4
    {
    b->M.p[count] += p->cmu * MAX(omega(i,j),0.0);
    
	b->rhsvec.V[count]  += Pk(i,j)
                    
                        + (1.0/sqrt(fabs(cf(i,j))>1.0e-20?cf(i,j):1.0e20))*pow(ustar(i,j),3.0)/HP;

	++count;
    }
}

void sflow_turb_kw_IM1::omega_source(lexer* p, fdm2D *b)
{
    count=0;
    SLICELOOP4
    {
    b->M.p[count] += kw_beta * MAX(omega(i,j),0.0);

    b->rhsvec.V[count] +=  kw_alpha * (MAX(omega(i,j),0.0)/(kin(i,j)>(1.0e-10)?(fabs(kin(i,j))):(1.0e20)))*Pk(i,j)
    
                       + (6.912/pow((fabs(cf(i,j))>1.0e-20?cf(i,j):1.0e20),0.75))*pow(p->cmu,1.5)*pow(ustar(i,j),4.0)*p->cmu*kin(i,j)/(HP*HP);
    ++count;
    }

}

void sflow_turb_kw_IM1::Pk_update(lexer* p, fdm2D *b, ghostcell *pgc)
{
    double dudx,dvdy,dudy,dvdx;
    
    SLICELOOP4
    {
    dudx=dvdy=dudy=dvdx=0.0;
    
    
	if(p->flagslice1[IJ]>0 && p->flagslice1[Im1J]>0)
    dudx = (b->P(i,j) - b->P(i-1,j))/(p->dx);
    
    if(p->flagslice2[IJ]>0 && p->flagslice2[IJm1]>0)
    dvdy = (b->Q(i,j) - b->Q(i,j-1))/(p->dx);
    
    if(p->flagslice1[IJp1]>0 && p->flagslice1[Im1Jp1]>0 && p->flagslice1[IJm1]>0 && p->flagslice1[Im1Jm1]>0)
    dudy = (0.5*(b->P(i,j+1)+b->P(i-1,j+1)) - 0.5*(b->P(i,j-1)+b->P(i-1,j-1)))/(2.0*p->dx);
    
    if(p->flagslice2[Ip1J]>0 && p->flagslice2[Ip1Jm1]>0 && p->flagslice2[Im1J]>0 && p->flagslice2[Im1Jm1]>0)
    dvdx = (0.5*(b->Q(i+1,j)+b->Q(i+1,j-1)) - 0.5*(b->Q(i-1,j)+b->Q(i-1,j-1)))/(2.0*p->dx);

    Pk(i,j) = b->eddyv(i,j)*(2.0*pow(dudx,2.0) + 2.0*pow(dvdy,2.0) + pow(dudy+dvdx,2.0));
    
    S(i,j) = sqrt(pow(dudx,2.0) + pow(dvdy,2.0) + 0.5*pow(dudy+dvdx,2.0));
    }
    
}

void sflow_turb_kw_IM1::ustar_update(lexer* p, fdm2D *b, ghostcell *pgc)
{
    double uvel,vvel,manning;
    
    SLICELOOP4
    {
    uvel = 0.5*(b->P(i,j) + b->P(i-1,j));
    vvel = 0.5*(b->Q(i,j) + b->Q(i,j-1));
    
    manning = pow(b->ks(i,j),1.0/6.0)/26.0;
    
    cf(i,j) = pow(manning,2.0)*9.81/pow(HP,1.0/3.0);
    
    ustar(i,j) = sqrt(cf(i,j)*(uvel*uvel + vvel*vvel));
    }
}

void sflow_turb_kw_IM1::timesource(lexer* p, fdm2D *b, slice &fn)
{
    count=0;
    SLICELOOP4
    {
        b->M.p[count] += 1.0/p->dt;

        b->rhsvec.V[count] += b->L(i,j) + fn(i,j)/p->dt + b->M.p[count]*fn(i,j)*(1.0/p->N55-1.0);
		
		b->M.p[count]/= p->N55;

	++count;
    }
}

void sflow_turb_kw_IM1::clearrhs(lexer* p, fdm2D *b)
{
    count=0;
    SLICELOOP4
    {
    b->rhsvec.V[count]=0.0;
	b->L(i,j)=0.0;
	++count;
    }
}












