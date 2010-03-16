/*
 * vim:expandtab:shiftwidth=8:tabstop=8:
 *
 * Copyright CEA/DAM/DIF  (2008)
 * contributeur : Philippe DENIEL   philippe.deniel@cea.fr
 *                Thomas LEIBOVICI  thomas.leibovici@cea.fr
 *
 *
 * Ce logiciel est un serveur implementant le protocole NFS.
 *
 * Ce logiciel est r�gi par la licence CeCILL soumise au droit fran�ais et
 * respectant les principes de diffusion des logiciels libres. Vous pouvez
 * utiliser, modifier et/ou redistribuer ce programme sous les conditions
 * de la licence CeCILL telle que diffus�e par le CEA, le CNRS et l'INRIA
 * sur le site "http://www.cecill.info".
 *
 * En contrepartie de l'accessibilit� au code source et des droits de copie,
 * de modification et de redistribution accord�s par cette licence, il n'est
 * offert aux utilisateurs qu'une garantie limit�e.  Pour les m�mes raisons,
 * seule une responsabilit� restreinte p�se sur l'auteur du programme,  le
 * titulaire des droits patrimoniaux et les conc�dants successifs.
 *
 * A cet �gard  l'attention de l'utilisateur est attir�e sur les risques
 * associ�s au chargement,  � l'utilisation,  � la modification et/ou au
 * d�veloppement et � la reproduction du logiciel par l'utilisateur �tant
 * donn� sa sp�cificit� de logiciel libre, qui peut le rendre complexe �
 * manipuler et qui le r�serve donc � des d�veloppeurs et des professionnels
 * avertis poss�dant  des  connaissances  informatiques approfondies.  Les
 * utilisateurs sont donc invit�s � charger  et  tester  l'ad�quation  du
 * logiciel � leurs besoins dans des conditions permettant d'assurer la
 * s�curit� de leurs syst�mes et ou de leurs donn�es et, plus g�n�ralement,
 * � l'utiliser et l'exploiter dans les m�mes conditions de s�curit�.
 *
 * Le fait que vous puissiez acc�der � cet en-t�te signifie que vous avez
 * pris connaissance de la licence CeCILL, et que vous en avez accept� les
 * termes.
 *
 * ---------------------
 *
 * Copyright CEA/DAM/DIF (2005)
 *  Contributor: Philippe DENIEL  philippe.deniel@cea.fr
 *               Thomas LEIBOVICI thomas.leibovici@cea.fr
 *
 *
 * This software is a server that implements the NFS protocol.
 * 
 *
 * This software is governed by the CeCILL  license under French law and
 * abiding by the rules of distribution of free software.  You can  use,
 * modify and/ or redistribute the software under the terms of the CeCILL
 * license as circulated by CEA, CNRS and INRIA at the following URL
 * "http://www.cecill.info".
 *
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability.
 *
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or
 * data to be ensured and,  more generally, to use and operate it in the
 * same conditions as regards security.
 *
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL license and that you accept its terms.
 * ---------------------------------------
 * Outils de mesure du temps
 *
 * $Header: /cea/home/cvs/cvs/SHERPA/BaseCvs/GANESHA/src/test/MesureTemps.c,v 1.3 2005/11/28 17:03:05 deniel Exp $
 *
 * $Log: MesureTemps.c,v $
 * Revision 1.3  2005/11/28 17:03:05  deniel
 * Added CeCILL headers
 *
 * Revision 1.2  2004/08/19 08:08:12  deniel
 * Mise au carre des tests sur les libs dynamiques et insertions des mesures
 * de temps dans les tests
 *
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <string.h>
#include "MesureTemps.h"

void MesureTemps(struct Temps *resultatp, struct Temps *basep)
{
  struct timeval t;

  gettimeofday(&t, NULL);
  if (basep == NULL)
    {
      resultatp->secondes = t.tv_sec;
      resultatp->micro_secondes = t.tv_usec;
    } else
    {
      long tempo;

      resultatp->secondes = t.tv_sec - basep->secondes;
      if ((tempo = t.tv_usec - basep->micro_secondes) < 0)
        {
          resultatp->secondes--;
          resultatp->micro_secondes = tempo + 1000000;
        } else
        {
          resultatp->micro_secondes = tempo;
        }
    }
}

char *ConvertiTempsChaine(struct Temps temps, char *resultat)
{
  static char chaine[100];
  char *ptr;

  if (resultat == NULL)
    {
      ptr = chaine;
    } else
    {
      ptr = resultat;
    }
  sprintf(ptr, "%u.%.6llu", (unsigned int)temps.secondes,
          (unsigned long long)temps.micro_secondes);
  return (ptr);
}

struct Temps *ConvertiChaineTemps(char *chaine, struct Temps *resultatp)
{
  static struct Temps temps;
  struct Temps *tp;
  char *ptr;

  if (resultatp == NULL)
    {
      tp = &temps;
    } else
    {
      tp = resultatp;
    }
  ptr = strchr(chaine, '.');
  if (ptr == NULL)
    {
      tp->secondes = atoi(chaine);
      tp->micro_secondes = 0;
    } else
    {
      *ptr = '\0';
      tp->secondes = atoi(chaine);
      tp->micro_secondes = atoi(ptr + 1);
    }
  return (tp);
}
