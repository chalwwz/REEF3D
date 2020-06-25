/*--------------------------------------------------------------------
REEF3D
Copyright 2008-2020 Hans Bihs

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

#include"etimestep.h"
#include<iomanip>
#include"lexer.h"
#include"fdm.h"
#include"ghostcell.h"
#include"turbulence.h"

etimestep::etimestep(lexer* p):epsi(1.0e-19),c0_orig(p->N47)
{
}

etimestep::~etimestep()
{
}

void etimestep::start(fdm *a, lexer *p, ghostcell *pgc, turbulence *pturb)
{
    p->umax=p->vmax=p->wmax=p->viscmax=irsm=jrsm=krsm=0.0;
    p->epsmax=p->kinmax=p->pressmax=0.0;
	p->dt_old=p->dt;

	p->umax=p->vmax=p->wmax=p->viscmax=0.0;
	sqd=1.0/(p->DXM*p->DXM);
	
// maximum velocities


	ULOOP
	p->umax=MAX(p->umax,fabs(a->u(i,j,k)));

	p->umax=pgc->globalmax(p->umax);


	VLOOP
	p->vmax=MAX(p->vmax,fabs(a->v(i,j,k)));

	p->vmax=pgc->globalmax(p->vmax);


	WLOOP
	p->wmax=MAX(p->wmax,fabs(a->w(i,j,k)));

	p->wmax=pgc->globalmax(p->wmax);

    if(p->mpirank==0 && (p->count%p->P12==0))
    {
	cout<<"umax: "<<setprecision(3)<<p->umax<<" \t utime: "<<p->utime<<endl;
	cout<<"vmax: "<<setprecision(3)<<p->vmax<<" \t vtime: "<<p->vtime<<endl;
	cout<<"wmax: "<<setprecision(3)<<p->wmax<<" \t wtime: "<<p->wtime<<endl;
    }
	
	p->umax=MAX(p->umax,p->ufbmax);
	p->vmax=MAX(p->vmax,p->vfbmax);
	p->wmax=MAX(p->wmax,p->wfbmax);

    velmax=max(p->umax,p->vmax,p->wmax);

// maximum viscosity
	LOOP
	p->viscmax=MAX(p->viscmax, a->visc(i,j,k)+a->eddyv(i,j,k));

	p->viscmax=pgc->globalmax(p->viscmax);

    if(p->mpirank==0 && (p->count%p->P12==0))
	cout<<"viscmax: "<<p->viscmax<<endl;
	//----kin
	LOOP
	p->kinmax=MAX(p->kinmax,pturb->kinval(i,j,k));

	p->kinmax=pgc->globalmax(p->kinmax);

    if(p->mpirank==0 && (p->count%p->P12==0))
	cout<<"kinmax: "<<p->kinmax<<endl;

	//---eps
    LOOP
	p->epsmax=MAX(p->epsmax,pturb->epsval(i,j,k));

	p->epsmax=pgc->globalmax(p->epsmax);

    if(p->mpirank==0 && (p->count%p->P12==0))
	cout<<"epsmax: "<<p->epsmax<<endl;


	//---press
    LOOP
    {
	p->pressmax=MAX(p->pressmax,a->press(i,j,k));
	p->pressmin=MIN(p->pressmin,a->press(i,j,k));
    }

	p->pressmax=pgc->globalmax(p->pressmax);
	p->pressmin=pgc->globalmin(p->pressmin);


// maximum reynolds stress source term
	visccrit=p->viscmax*(6.0/pow(p->DXM,2.0));  
 
    cu=cv=cw=1.0e10;
    
    ULOOP
    {
    dx = MIN3(p->DXP[IP],p->DYN[JP],p->DZN[KP]);
    
    visc = 0.5*(a->eddyv(i,j,k) + a->eddyv(i+1,j,k)) + 0.5*(a->visc(i,j,k) + a->visc(i+1,j,k));
    
	cu = MIN(cu, 2.0/((p->umax/dx +  visc*(6.0/pow(dx,2.0)))
    
            +sqrt(pow(p->umax/dx+visc,2.0)+(4.0*fabs(fabs(a->gi)+a->maxF))/dx)));
    }

    VLOOP
    {
    dx = MIN3(p->DXN[IP],p->DYP[JP],p->DZN[KP]);
    
    visc = 0.5*(a->eddyv(i,j,k) + a->eddyv(i,j+1,k)) + 0.5*(a->visc(i,j,k) + a->visc(i,j+1,k));
    
	cv = MIN(cv, 2.0/((p->vmax/dx +  visc*(6.0/pow(dx,2.0)))
    
            +sqrt(pow(p->vmax/dx+visc,2.0)+(4.0*fabs(fabs(a->gj)+a->maxG))/dx)));
    }
    
    WLOOP
    {
    dx = MIN3(p->DXN[IP],p->DYN[JP],p->DZP[KP]);
    
    visc = 0.5*(a->eddyv(i,j,k) + a->eddyv(i,j,k+1)) + 0.5*(a->visc(i,j,k) + a->visc(i,j,k+1));
    
	cw = MIN(cw, 2.0/((p->wmax/dx +  visc*(6.0/pow(dx,2.0)))
    
            +sqrt(pow(p->wmax/dx+visc,2.0)+(4.0*fabs(fabs(a->gk)+a->maxH))/dx)));
    }
    
    
    cu = min(cu,cv,cw);
    
    LOOP
    {
    dx = MIN3(p->DXP[IP],p->DYN[JP],p->DZN[KP]);
    
    visc = 0.5*(a->eddyv(i,j,k) + a->eddyv(i+1,j,k)) + 0.5*(a->visc(i,j,k) + a->visc(i+1,j,k));
    
	cu = MIN(cu, 2.0/((sqrt(p->umax*p->umax + p->vmax*p->vmax + p->wmax*p->wmax)/dx +  visc*(6.0/pow(dx,2.0)))
    
            +sqrt(pow(sqrt(p->umax*p->umax + p->vmax*p->vmax + p->wmax*p->wmax)/dx+visc,2.0)
            
            + (4.0*fabs(fabs(a->gi) + MAX3(a->maxF,a->maxG,a->maxH)))/dx)));
    }
    

	p->dt=p->N47*min(cu,cv,cw);
	p->dt=pgc->timesync(p->dt);

	a->maxF=0.0;
	a->maxG=0.0;
	a->maxH=0.0;
}

void etimestep::ini(fdm* a, lexer* p,ghostcell* pgc)
{

	p->umax=p->vmax=p->wmax=p->viscmax=-1e19;

	p->umax=p->W10;
	
	p->umax=MAX(p->umax,2.0*p->ufbmax);
	p->umax=MAX(p->umax,2.0*p->vfbmax);
	p->umax=MAX(p->umax,2.0*p->wfbmax);
    
    p->umax=MAX(p->umax,2.0*p->X210_u);
	p->umax=MAX(p->umax,2.0*p->X210_v);
	p->umax=MAX(p->umax,2.0*p->X210_w);
    
    p->umax+=2.0;


	p->dt=p->DXM/(p->umax+epsi);

	LOOP
	p->viscmax=MAX(p->viscmax, a->visc(i,j,k)+a->eddyv(i,j,k));

	visccrit=(p->viscmax*(6.0/pow(p->DXM,2.0)));

	dx = p->DXM;
    
    LOOP
    dx = MIN(dx,MIN3(p->DXP[IP],p->DYN[JP],p->DZN[KP]));
    
    cu=2.0/((p->umax/dx+visccrit)+sqrt(pow(p->umax/dx+visccrit,2.0)+(4.0*sqrt(fabs(a->gi) + fabs(a->gj) +fabs(a->gk)))/dx));// + (8.0*p->maxkappa*p->W5)/(2.0*dx*dx*(p->W1+p->W3)));

	p->dt=p->N47*cu*0.25;
	p->dt=pgc->timesync(p->dt);
	p->dt_old=p->dt;
}

double etimestep::min(double val1,double val2,double val3)
{
	double mini;

	mini=val1;

	if(mini>val2)
	mini=val2;

	if(mini>val3)
	mini=val3;

	if(mini<0.0)
	mini=0.0;

	return mini;
}

double etimestep::min(double val1,double val2)
{
	double mini;

	mini=val1;

	if(mini>val2)
	mini=val2;

	if(mini<0.0)
	mini=0.0;

	return mini;
}

double etimestep::max(double val1,double val2,double val3)
{
	double maxi;

	maxi=val1;

	if(maxi<val2)
	maxi=val2;

	if(maxi<val3)
	maxi=val3;

	if(maxi<0.0)
	maxi=0.0;

	return maxi;
}

double etimestep::max(double val1,double val2)
{
	double maxi;

	maxi=val1;

	if(maxi<val2)
	maxi=val2;

	if(maxi<0.0)
	maxi=0.0;

	return maxi;
}
