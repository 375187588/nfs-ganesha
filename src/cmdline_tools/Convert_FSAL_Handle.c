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
 * ---------------------------------------*/

/**
 *
 * \file    main.c
 * \author  $Author: leibovic $
 * \date    $Date: 2006/02/23 07:42:53 $
 * \version $Revision: 1.28 $
 * \brief   extract fileid from FSAL handle.
 *
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include "log_functions.h"
#include "stuff_alloc.h"
#include "fsal.h"

#ifdef _USE_HPSS_62
#include "../FSAL/FSAL_HPSS/HPSSclapiExt/hpssclapiext.h"
#endif

#define CMD_BUFFER_SIZE 1024

#define CONFIG_FILE "/var/hpss/etc/hpss.ganesha.nfsd.conf"

time_t          ServerBootTime ;


int main( int argc, char * argv[] ) 
{
 int                       c ;
 char                      exec_name[MAXPATHLEN] ;
 char                    * tempo_exec_name       = NULL ;
 
#ifdef _USE_HPSS_62
 char                      buffer[CMD_BUFFER_SIZE] ; 
 fsal_handle_t             fsal_handle;
 char                      str[2*CMD_BUFFER_SIZE] ;
 uint64_t                 objid ;
 ns_ObjHandle_t           hpss_hdl;
 hpss_Attrs_t             hpss_attr;
 char                   * tmp_str_uuid ;
#endif
 char                     options[] = "h@" ;
 char                     usage[] = "%s [-h] <FSAL_Handle>\n"
                                    "   -h               : prints this help\n";

 ServerBootTime = time( NULL ) ;

 /* What is the executable file's name */
 if( ( tempo_exec_name = strrchr( argv[0], '/' ) ) != NULL )
   strcpy( (char *)exec_name, tempo_exec_name + 1 ) ;

 /* now parsing options with getopt */
 while( ( c = getopt( argc, argv, options ) ) != EOF )
    {
     switch( c ) 
      {
	case '@':
	  printf( "%s compiled on %s at %s\n", exec_name, __DATE__, __TIME__ ) ;
          exit( 0 ) ;
	  break ;
  
	case 'h':
	   printf( usage, exec_name ) ;
	   exit( 0 ) ;
	   break ;
	  
    case '?':
       printf("Unknown option: %c\n", optopt );
       printf( usage, exec_name ) ;
       exit(1);
      }		
    }

  if ( optind != argc - 1 )
  {
        printf("Missing argument: <FSAL_Handle>\n");
        printf( usage, exec_name ) ;
        exit(1);
  }

#ifdef _USE_HPSS_62 
  sscanHandle(&fsal_handle,argv[optind]);
   
  snprintmem( (caddr_t)str, 2*CMD_BUFFER_SIZE, (caddr_t)&fsal_handle.ns_handle, sizeof( ns_ObjHandle_t) )  ;
  printf( "NS Handle = %s\n", str ) ;

  objid = hpss_GetObjId( &(fsal_handle.ns_handle) ) ;
  printf( "FileId = %llu\n", objid ) ;
#endif 
  

 exit( 0 ) ;
}
