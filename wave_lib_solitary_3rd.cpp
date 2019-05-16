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

#include"wave_lib_solitary_3rd.h"
#include"lexer.h"
#include"fdm.h"
#include"ghostcell.h"

wave_lib_solitary_3rd::wave_lib_solitary_3rd(lexer *p, ghostcell *pgc) : wave_lib_parameters(p,pgc) 
{ 
    parameters(p,pgc);
    
    if(p->mpirank==0)
    {
    cout<<"Wave Tank: 3rd-order solitary wave; ";
    cout<<"wk: "<<wk<<" ww: "<<ww<<" wf: "<<wf<<" wT: "<<wT<<" wL: "<<wL<<" wd: "<<wd<<endl;
    }
    
    singamma = sin((p->B105_1)*(PI/180.0));
    cosgamma = cos((p->B105_1)*(PI/180.0));
}

wave_lib_solitary_3rd::~wave_lib_solitary_3rd()
{
}

double wave_lib_solitary_3rd::wave_u(lexer *p, double x, double y, double z)
{
    double vel;
	
	vel = wave_horzvel(p,x,y,z);
	
    return cosgamma*vel;
}

double wave_lib_solitary_3rd::wave_v(lexer *p, double x, double y, double z)
{
    double vel;
	
	vel = wave_horzvel(p,x,y,z);
	
    return singamma*vel;
}

double wave_lib_solitary_3rd::wave_horzvel(lexer *p, double x, double y, double z)
{
    double vel,eta,e,s,t,aval;
	
	teta = -(wC*(p->simtime) - (x - X0));
	
	e = wH/wd;
	
	aval = (1.0/wd)*sqrt(0.75*e) * (1.0 - (5.0/8.0)*e + (71.0/128.0)*e*e);
	
	s = 1.0/cosh(aval*teta);
	
	t = tanh(aval*teta);
		
	vel = sqrt(9.81*wd)*(e*s*s + e*e*(-0.75*s*s + s*s*t*t + pow((wd+z)/wd,2.0)*(0.75*s*s - (9.0/4.0)*s*s*t*t)
						+ e*e*e*((21.0/40.0)*s*s - s*s*t*t - (6.0/5.0)*s*s*s*s*t*t + pow((wd+z)/wd,2.0)*(-(9.0/4.0)*s*s + (15.0/4.0)*s*s*t*t + (15.0/2.0)*s*s*s*s*t*t)
								 + pow((wd+z)/wd,4.0) * (3.0/8.0)*s*s - (45.0/16.0)*s*s*s*s*t*t)));
		
	if(z<-(p->phimean-p->B126))
	vel=0.0;
	
    return vel;
}

double wave_lib_solitary_3rd::wave_w(lexer *p, double x, double y, double z)
{
    double vel,eta,e,s,t,aval;
	
	teta = -(wC*(p->simtime) - (x - X0));
	
	e = wH/wd;
	
	aval = (1.0/wd)*sqrt(0.75*e) * (1.0 - (5.0/8.0)*e + (71.0/128.0)*e*e);
	
	s = 1.0/cosh(aval*teta);
	
	t = tanh(aval*teta);
			
	vel = sqrt(9.81*wd)*sqrt(3.0*e)*((wd+z)/wd)*s*s*t*( e
		+ e*e*((3.0/8.0)*s*s + 2.0*s*s + pow((wd+z)/wd,2.0)*((-1.0/2.0) + 1.5*s*s))
		+ e*e*e*((49.0/640.0) -(17.0/20.0)*s*s - (18.0/5.0)*s*s*s*s 
			+ pow((wd+z)/wd,2.0)*(-(13.0/16.0) - (25.0/16.0)*s*s + (15.0/2.0)*s*s*s*s)
			+ pow((wd+z)/wd,4.0)*(-(3.0/40.0) + (9.0/8.0)*s*s - (27.0/16.0)*s*s*s*s)));
			
	if(z<-(p->phimean-p->B126))
	vel=0.0;
		
    return vel;
}

double wave_lib_solitary_3rd::wave_eta(lexer *p, double x, double y)
{
    double eta,e,t,s,aval;

	
	teta = -(wC*(p->simtime) - (x - X0));
	
	e = wH/wd;
	
	aval = (1.0/wd)*sqrt(0.75*e) * (1.0 - (5.0/8.0)*e + (71.0/128.0)*e*e);
	
	s = 1.0/cosh(aval*teta);
	
	t = tanh(aval*teta);
	
	eta =  wd*(e*s*s - 0.75*e*e*s*s*t*t + e*e*e*((5.0/8.0)*s*s*t*t - (101.0/80.0)*s*s*s*s*t*t));
	
	return eta;	
}

double wave_lib_solitary_3rd::wave_fi(lexer *p, double x, double y, double z, int q)
{
    double fi;
    
    return fi;
}

void wave_lib_solitary_3rd::parameters(lexer *p, ghostcell *pgc)
{
    wH = wa;
	p->wH = wa;
	
	double e = wH/wd;
	
	wC = sqrt(9.81*wd)*sqrt(1.0 + e  - (1.0/20.0)*e*e - (3.0/70.0)*e*e*e);
	
	if(p->mpirank==0)
	cout<<"wC 1st: "<<sqrt(9.81*(wH+wd))<<" | wC Grimshaw: "<<wC;
	

	if(p->mpirank==0)
	cout<<" | wC Fenton: "<<sqrt(9.81*wd)*sqrt(1.0 + 0.5*e  - (3.0/20.0)*e*e - (3.0/56.0)*e*e*e)<<endl;
	
	X0 = - (2.12*wd)/(sqrt((0.5*wa)/wd));
	
	
	if(p->mpirank==0)
	cout<<"X0: "<<X0<<endl;
    
}