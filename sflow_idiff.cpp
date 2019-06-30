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

#include"sflow_idiff.h"
#include"lexer.h"
#include"ghostcell.h"
#include"fdm2D.h"
#include"solver2D.h"

sflow_idiff::sflow_idiff(lexer* p)
{
    gcval_u = 10;
    gcval_v = 11;
}

sflow_idiff::~sflow_idiff()
{
}

void sflow_idiff::diff_u(lexer* p, fdm2D *b, ghostcell *pgc, solver2D *psolv, slice &u, slice &v, double alpha)
{
    starttime=pgc->timer();
    
    n=0;
    SLICELOOP1
    {
	visc = p->W2 + 0.5*(b->eddyv(i,j) + b->eddyv(i+1,j));

        
	b->M.p[n] =  6.0*visc/(p->dx*p->dx)
                   
				   + 1.0/(alpha*p->dt);
    
	b->rhsvec.V[n] = (visc/(p->dx*p->dx))*((v(i+1,j)-v(i,j)) - (v(i+1,j-1)-v(i,j-1)))
									
						 + b->M.p[n]*u(i,j)*(1.0/p->N54-1.0)
                         
						 + (u(i,j))/(alpha*p->dt);

	 b->M.p[n] /= p->N54;
	 
	 b->M.s[n] = -2.0*visc/(p->dx*p->dx);
	 b->M.n[n] = -2.0*visc/(p->dx*p->dx);
	 
	 b->M.e[n] = -visc/(p->dx*p->dx);
	 b->M.w[n] = -visc/(p->dx*p->dx);
 
	 ++n;
	}
    
    
    n=0;
    SLICELOOP1
    {
        if(p->flagslice1[Im1J]<0)
		{
		b->rhsvec.V[n] -= b->M.s[n]*u(i-1,j);
		b->M.s[n] = 0.0;
		}
		
		if(p->flagslice1[Ip1J]<0)
		{
		b->rhsvec.V[n] -= b->M.n[n]*u(i+1,j);
		b->M.n[n] = 0.0;
		}
		
		if(p->flagslice1[IJm1]<0)
		{
		b->rhsvec.V[n] -= b->M.e[n]*u(i,j-1);
		b->M.e[n] = 0.0;
		}
		
		if(p->flagslice1[IJp1]<0)
		{
		b->rhsvec.V[n] -= b->M.w[n]*u(i,j+1);
		b->M.w[n] = 0.0;
		}
 
	++n;
	}
    
	psolv->start(p,pgc,u,b->M,b->xvec,b->rhsvec,1,gcval_u,p->D29,b->C1);
    
    pgc->gcsl_start1(p,u,gcval_u);
    
	time=pgc->timer()-starttime;
	p->uiter=p->solveriter;
	if(p->mpirank==0 && innercounter==p->N50-1 && p->D21==1 && (p->count%p->P12==0))
	cout<<"udiffiter: "<<p->uiter<<"  udifftime: "<<setprecision(3)<<time<<endl;
}

void sflow_idiff::diff_v(lexer* p, fdm2D *b, ghostcell *pgc, solver2D *psolv, slice &u, slice &v, double alpha)
{
    starttime=pgc->timer();
   
    n=0;
    SLICELOOP2
    {
	visc = p->W2 + 0.5*(b->eddyv(i,j) + b->eddyv(i,j+1));

        
	b->M.p[n] =  6.0*visc/(p->dx*p->dx)
                   
				   + 1.0/(alpha*p->dt);
				  
	b->rhsvec.V[n] = (visc/(p->dx*p->dx))*((u(i,j+1)-u(i,j)) - (u(i-1,j+1)-u(i-1,j)))
									
						 + b->M.p[n]*v(i,j)*(1.0/p->N54-1.0)
						 + (v(i,j))/(alpha*p->dt);
									
	 b->M.p[n] /= p->N54;
	 
	 b->M.s[n] = -visc/(p->dx*p->dx);
	 b->M.n[n] = -visc/(p->dx*p->dx);
	 
	 b->M.e[n] = -2.0*visc/(p->dx*p->dx);
	 b->M.w[n] = -2.0*visc/(p->dx*p->dx);
 
	 ++n;
	}
    
    n=0;
    SLICELOOP2
    {
        if(p->flagslice2[Im1J]<0)
		{
		b->rhsvec.V[n] -= b->M.s[n]*v(i-1,j);
		b->M.s[n] = 0.0;
		}
		
		if(p->flagslice2[Ip1J]<0)
		{
		b->rhsvec.V[n] -= b->M.n[n]*v(i+1,j);
		b->M.n[n] = 0.0;
		}
		
		if(p->flagslice2[IJm1]<0)
		{
		b->rhsvec.V[n] -= b->M.e[n]*v(i,j-1);
		b->M.e[n] = 0.0;
		}
		
		if(p->flagslice2[IJp1]<0)
		{
		b->rhsvec.V[n] -= b->M.w[n]*v(i,j+1);
		b->M.w[n] = 0.0;
		}
 
	++n;
	}
    
	psolv->start(p,pgc,v,b->M,b->xvec,b->rhsvec,2,gcval_v,p->D29,b->C2);
    
    pgc->gcsl_start2(p,v,gcval_v);
    
	time=pgc->timer()-starttime;
	p->viter=p->solveriter;
	if(p->mpirank==0 && innercounter==p->N50-1 && p->D21==1 && (p->count%p->P12==0))
	cout<<"vdiffiter: "<<p->uiter<<"  vdifftime: "<<setprecision(3)<<time<<endl;
}

void sflow_idiff::diff_scalar(lexer* p, fdm2D *b, ghostcell *pgc, solver2D *psolv, slice &f, double sig, double alpha)
{
    count=0;

    sqd = (1.0/(p->dx*p->dx));

	SLICELOOP4
	{
	ev_ij=b->eddyv(i,j);
	visc_ij=p->W2;

	b->M.p[count]  +=   0.5*sqd*(vft*visc_ij+b->eddyv(i+1,j)/sig + vft*visc_ij+ev_ij/sig)
    
					+   0.5*sqd*(vft*visc_ij+ev_ij/sig + vft*visc_ij+b->eddyv(i-1,j)/sig)
                    
					+   0.5*sqd*(vft*visc_ij+b->eddyv(i,j+1)/sig + vft*visc_ij+ev_ij/sig)
                    
					+   0.5*sqd*(vft*visc_ij+ev_ij/sig + vft*visc_ij+b->eddyv(i,j-1)/sig);

	 
	 b->M.s[count] -= 0.5*sqd*(vft*visc_ij+ev_ij/sig + vft*visc_ij+b->eddyv(i-1,j)/sig);
	 b->M.n[count] -= 0.5*sqd*(vft*visc_ij+b->eddyv(i+1,j)/sig + vft*visc_ij+ev_ij/sig);
	 
	 b->M.e[count] -= 0.5*sqd*(vft*visc_ij+ev_ij/sig + vft*visc_ij+b->eddyv(i,j-1)/sig);
	 b->M.w[count] -= 0.5*sqd*(vft*visc_ij+b->eddyv(i,j+1)/sig + vft*visc_ij+ev_ij/sig);

	 ++count;
	}
}