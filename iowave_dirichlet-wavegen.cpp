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

#include"iowave.h"
#include"lexer.h"
#include"fdm.h"
#include"ghostcell.h"


void iowave::dirichlet_wavegen(lexer *p, fdm* a, ghostcell* pgc, field& u, field& v, field& w)
{
        count=0;
		for(n=0;n<p->gcin_count;n++)
		{
		i=p->gcin[n][0];
		j=p->gcin[n][1];
		k=p->gcin[n][2];		

        uvel=uval[count]*ramp(p);
        vvel=vval[count]*ramp(p);
        wvel=wval[count]*ramp(p);
        
        
        //cout<<"UVAL: "<<uval[count]<<" k: "<<k<<endl;
            

			if(a->phi(i-1,j,k)>=0.0)
			{
            
			u(i-1,j,k)=uvel+p->Ui;
			u(i-2,j,k)=uvel+p->Ui;
			u(i-3,j,k)=uvel+p->Ui;
            
            v(i-1,j,k)=vvel;
			v(i-2,j,k)=vvel;
			v(i-3,j,k)=vvel;
			
			w(i-1,j,k)=wvel;
			w(i-2,j,k)=wvel;
			w(i-3,j,k)=wvel;
			}

			if(a->phi(i-1,j,k)<0.0 && a->phi(i-1,j,k)>=-p->F45*p->DZP[KP])
			{
			fac= p->B122*(1.0 - fabs(a->phi(i-1,j,k))/(p->F45*p->DZP[KP]));
            
			u(i-1,j,k)=uvel*fac + p->Ui;
			u(i-2,j,k)=uvel*fac + p->Ui;
			u(i-3,j,k)=uvel*fac + p->Ui;
            
            v(i-1,j,k)=vvel*fac;
			v(i-2,j,k)=vvel*fac;
			v(i-3,j,k)=vvel*fac;
            
			w(i-1,j,k)=wvel*fac;
			w(i-2,j,k)=wvel*fac;
			w(i-3,j,k)=wvel*fac;
			}

			if(a->phi(i-1,j,k)<-p->F45*p->DXM)
			{
			//pgc->dirichlet_ortho(p,u,p->dx,10,1,1);
			u(i-1,j,k)=0.0 + p->Ui;
			u(i-2,j,k)=0.0 + p->Ui;
			u(i-3,j,k)=0.0 + p->Ui;
            
            v(i-1,j,k)=0.0;
			v(i-2,j,k)=0.0;
			v(i-3,j,k)=0.0;

			w(i-1,j,k)=0.0;
			w(i-2,j,k)=0.0;
			w(i-3,j,k)=0.0;
			}
        ++count;
		}
        
        
        if(p->B98==3||p->B98==4||p->B99==3||p->B99==4||p->B99==5)
		if(p->B64==1)
		{
		for(int q=0;q<4;++q)
		for(n=0;n<p->gcin_count;++n)
		{
		i=p->gcin[n][0]+q;
		j=p->gcin[n][1];
		k=p->gcin[n][2];

		if(a->phi(i,j,k)<0.0)
		a->eddyv(i,j,k)=MIN(a->eddyv(i,j,k),1.0e-4);
		}
		pgc->start4(p,a->eddyv,24);
		}
        
        
    // NSEWAVE
    if(p->A10==4)
    {
        for(n=0;n<gcgen4_count;++n)
		{
		i=gcgen4[n][0];
		j=gcgen4[n][1];
        
        x=xgen(p);
        y=ygen(p);
        
        a->eta(i-1,j)=a->eta(i-2,j)=a->eta(i-3,j)=wave_eta(p,pgc,x,y);        
            
            KLOOP
            {
            a->phi(i-1,j,k) = a->eta(i-1,j) + p->phimean - p->pos_z();
            a->phi(i-2,j,k) = a->eta(i-2,j) + p->phimean - p->pos_z();
            a->phi(i-3,j,k) = a->eta(i-3,j) + p->phimean - p->pos_z();
            }
        }
        
        

        for(n=0;n<gcgen1_count;++n)
		{
		i=gcgen1[n][0];
		j=gcgen1[n][1];
            
            a->P(i-1,j)=0.0;
            double d=0.0;
            double epsi=p->A440*p->dx;
            
            KULOOP
            {
            phival = 0.5*(a->phi(i,j,k)+a->phi(i-1,j,k));
            
                if(phival>epsi)
                H=1.0;

                if(phival<-epsi)
                H=0.0;

                if(fabs(phival)<=epsi)
                H=0.5*(1.0 + phival/epsi + (1.0/PI)*sin((PI*phival)/epsi));
                
                a->P(i-1,j) += a->u(i-1,j,k)*p->dx*H;
                d+=p->dx*H;
            }
            a->P(i-1,j)/=d;
            a->P(i-3,j)=a->P(i-2,j)=a->P(i-1,j);

        }
        
    }
}