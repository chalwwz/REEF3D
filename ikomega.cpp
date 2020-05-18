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

#include"ikomega.h"
#include"ghostcell.h"
#include"lexer.h"
#include"fdm.h"
#include"vrans_f.h"
#include"vrans_v.h"

ikomega::ikomega(lexer* p, fdm* a, ghostcell *pgc) : rans_io(p,a),bc_ikomega(p)
{
    if(p->B269==0)
	pvrans = new vrans_v(p,a,pgc);
	
	if(p->B269>=1 || p->S10==2)
	pvrans = new vrans_f(p,a,pgc);
}

ikomega::~ikomega()
{
}

void  ikomega::clearfield(lexer *p, fdm*  a, field& b)
{
	LOOP
	b(i,j,k)=0.0;
}

void ikomega::isource(lexer *p, fdm* a)
{
	ULOOP
	a->F(i,j,k)=0.0;
}

void ikomega::jsource(lexer *p, fdm* a)
{
	VLOOP
	a->G(i,j,k)=0.0;
}

void ikomega::ksource(lexer *p, fdm* a)
{
	WLOOP
	a->H(i,j,k)=0.0;
}

void  ikomega::eddyvisc(lexer* p, fdm* a, ghostcell* pgc)
{
	double factor;
	double H;
	double epsi = 1.6*p->DXM;
	int n;
	
	if(p->T30==0)
	LOOP
	a->eddyv(i,j,k) = kin(i,j,k)/eps(i,j,k);


    if(p->T30==1)
	LOOP
    {
	a->eddyv(i,j,k) = MAX(MAX(kin(i,j,k)
					  /((eps(i,j,k))>(1.0e-20)?(eps(i,j,k)):(1.0e20)),0.0),
					  0.0001*a->visc(i,j,k));
	}

	if(p->T30==2)
	{
		LOOP
		{
			if(a->phi(i,j,k)>epsi)
			H=1.0;

			if(a->phi(i,j,k)<-epsi)
			H=0.0;

			if(fabs(a->phi(i,j,k))<=epsi)
			H=0.5*(1.0 + a->phi(i,j,k)/epsi + (1.0/PI)*sin((PI*a->phi(i,j,k))/epsi));
			
			factor = H*p->T31 + (1.0-H)*p->T32;
			
		a->eddyv(i,j,k) = MAX(MIN(MAX(kin(i,j,k)
						  /((eps(i,j,k))>(1.0e-20)?(eps(i,j,k)):(1.0e20)),0.0),fabs(factor*kin(i,j,k))/strainterm(p,a)),
						  0.0001*a->visc(i,j,k));
		}
		
		GC4LOOP
		if(p->gcb4[n][4]==21 || p->gcb4[n][4]==22 || p->gcb4[n][4]==5)
		{
		i = p->gcb4[n][0];
		j = p->gcb4[n][1];
		k = p->gcb4[n][2];
		
		a->eddyv(i,j,k) = MAX(MIN(MAX(kin(i,j,k)
						  /((eps(i,j,k))>(1.0e-20)?(eps(i,j,k)):(1.0e20)),0.0),fabs(p->T35*kin(i,j,k))/strainterm(p,a)),
						  0.0001*a->visc(i,j,k));
		}
	}
	
	if(p->T30==3)
	{
		LOOP
		{
			if(a->phi(i,j,k)>epsi)
			H=1.0;

			if(a->phi(i,j,k)<-epsi)
			H=0.0;

			if(fabs(a->phi(i,j,k))<=epsi)
			H=0.5*(1.0 + a->phi(i,j,k)/epsi + (1.0/PI)*sin((PI*a->phi(i,j,k))/epsi));
			
			factor = H*p->T31 + (1.0-H)*p->T32;
			
			double epslim = MAX(eps(i,j,k), (factor*sqrt(2.0)*strainterm(p,a))/sqrt(0.09));
			
			
		a->eddyv(i,j,k) = MAX(kin(i,j,k)/(epslim>1.0e-20?epslim:1.0e20),0.0001*a->visc(i,j,k));
						  
		}
		
	}
	
	if(p->T10==22)
	LOOP
	a->eddyv(i,j,k) = MIN(a->eddyv(i,j,k), p->DXM*p->cmu*pow((kin(i,j,k)>(1.0e-20)?(kin(i,j,k)):(1.0e20)),0.5));
	
    
    pvrans->eddyv_func(p,a);
    
	pgc->start4(p,a->eddyv,29);
    
    n=0;
    LOOP
    {
    a->visctot.V[I_J_K] = a->visc(i,j,k) + a->eddyv(i,j,k);
    ++n;
    }

}

void  ikomega::kinsource(lexer *p, fdm* a)
{	
    int count=0;

    if(p->T40==0)
    LOOP
    {

	if(wallf(i,j,k)==0)
    a->M.p[count] += p->cmu*eps(i,j,k);
	
	if(wallf(i,j,k)==0)
	a->rhsvec.V[count]  += pk(p,a);

	++count;
    }

    if(p->T40==1)
    LOOP
    {
	if(wallf(i,j,k)==0)
	a->M.p[count] += p->cmu * MAX(eps(i,j,k),0.0);
	
	if(wallf(i,j,k)==0)
	a->rhsvec.V[count]  += pk(p,a);

	++count;
    }

    if(p->T40==2 || p->T40==3)
    LOOP
    {
	if(wallf(i,j,k)==0)
	a->M.p[count] += p->cmu * MAX(eps(i,j,k),0.0);
	
	if(wallf(i,j,k)==0)
	a->rhsvec.V[count]  += MIN(pk(p,a), p->T42*p->cmu*MAX(kin(i,j,k),0.0)*MAX(eps(i,j,k),0.0));
	
    if(wallf(i,j,k)==1)
	a->rhsvec.V[count]  = 0.0;

	++count;
    }
    
    pvrans->kw_source(p,a,kin);
}

void  ikomega::epssource(lexer *p, fdm* a)
{
    count=0;
    double dirac;


	if(p->T41==1)
	{
        if(p->T40==0)
        LOOP
        {
		a->M.p[count] += kw_beta * eps(i,j,k);

        a->rhsvec.V[count] += kw_alpha * (eps(i,j,k)/kin(i,j,k))*pk(p,a);
		 
        ++count;
        }

        if(p->T40==1 || p->T40==2)
        LOOP
        {
		a->M.p[count] += kw_beta * MAX(eps(i,j,k),0.0);

        a->rhsvec.V[count] +=  kw_alpha * (MAX(eps(i,j,k),0.0)/(kin(i,j,k)>(1.0e-10)?(fabs(kin(i,j,k))):(1.0e20)))*pk(p,a);
        ++count;
        }

        if(p->T40==3)
        LOOP
        {
		a->M.p[count] += kw_beta * MAX(eps(i,j,k),0.0);

        a->rhsvec.V[count] +=  kw_alpha * (MAX(eps(i,j,k),0.0)/(kin(i,j,k)>(1.0e-10)?(fabs(kin(i,j,k))):(1.0e20)))
									   * MIN(pk(p,a), p->T42*p->cmu*MAX(kin(i,j,k),0.0)*MAX(eps(i,j,k),0.0));
        ++count;
        }
	}

	if(p->T41==2)
	{
        if(p->T40==0)
        LOOP
        {
        a->M.p[count] += kw_beta * eps(i,j,k);

        a->rhsvec.V[count] +=  kw_alpha * pk_w(p,a);
        ++count;
        }

        if(p->T40==1 || p->T40==2)
        LOOP
        {
        a->M.p[count] += kw_beta * MAX(eps(i,j,k),0.0);

        a->rhsvec.V[count] += kw_alpha * pk_w(p,a);
        ++count;
        }

        if(p->T40==3)
        LOOP
        {
        a->M.p[count] += kw_beta * MAX(eps(i,j,k),0.0);

        a->rhsvec.V[count] +=  kw_alpha * MIN(pk_w(p,a), p->T42*p->cmu*MAX(kin(i,j,k),0.0)*MAX(eps(i,j,k),0.0)/MAX(a->eddyv(i,j,k),0.0001*a->visc(i,j,k)));
        ++count;
        }
    }
    
    pvrans->omega_source(p,a,kin,eps);
}

void ikomega::epsfsf(lexer *p, fdm* a, ghostcell *pgc)
{
	double epsi = p->T38*p->DXM;
	double dirac;

	
	if(p->T36>0)
	LOOP
	{
            if(p->j_dir==0)
            epsi = p->T38*(1.0/2.0)*(p->DXN[IP]+p->DZN[KP]);
            
            if(p->j_dir==1)
            epsi = p->T38*(1.0/3.0)*(p->DXN[IP]+p->DYN[JP]+p->DZN[KP]);
        
        
		if(fabs(a->phi(i,j,k))<epsi)
		dirac = (0.5/epsi)*(1.0 + cos((p->T39*PI*a->phi(i,j,k))/epsi));
		
		if(fabs(a->phi(i,j,k))>=epsi)
		dirac=0.0;
        
	
	if(dirac>0.0 && p->T36==1)
	eps(i,j,k) = dirac*2.5*pow(p->cmu,-0.25)*pow(fabs(kin(i,j,k)),0.5)*(1.0/p->T37);

	if(dirac>0.0 && p->T36==2)
	eps(i,j,k) = dirac*2.5*pow(p->cmu,-0.25)*pow(fabs(kin(i,j,k)),0.5)*(1.0/p->T37 + 1.0/(a->walld(i,j,k)>1.0e-20?a->walld(i,j,k):1.0e20));
	}
}




