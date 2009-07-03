/*
 *
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
 */

/**
 * \file    fsal.h
 * \author  $Author: leibovic $
 * \date    $Date: 2006/02/17 13:41:01 $
 * \version $Revision: 1.72 $
 * \brief   File System Abstraction Layer interface.
 *
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/* fsal_types contains constants and type definitions for FSAL */
#include "log_functions.h"
#include "stuff_alloc.h"
#include "fsal_types.h"
#include "fsal.h"
#include "mfsl_types.h"
#include "mfsl.h"
#include "common_utils.h"

#include <pthread.h>
#include <errno.h>

#ifndef _USE_SWIG

extern mfsl_parameter_t  mfsl_param ;

fsal_handle_t      dir_handle_precreate ;
unsigned int       dir_handle_set = 0 ;
log_t              log_outputs  = LOG_INITIALIZER ;  /**< Log descriptor   */
unsigned int       end_of_mfsl = FALSE ;

void constructor_preacreated_entries( void * ptr )
{
  mfsl_precreated_object_t * pobject = ( mfsl_precreated_object_t *)ptr ;
  
  pobject->inited = 0 ;
} /* constructor_preacreated_entries */

/**
 * 
 * mfsl_async_init_clean_precreated_objects: deletes every previously allocated object (allocation done by a former instance of the server).
 *
 * Deletes every previously allocated object (allocation done by a former instance of the server).
 *
 * @param pcontext    [INOUT] pointer to FSAL context to be used 
 *
 * @return a FSAL status
 *
 */
#define NB_DIRENT_CLEAN 100 

fsal_status_t mfsl_async_init_clean_precreated_objects( fsal_op_context_t  * pcontext )
{
  fsal_status_t        fsal_status ;
  fsal_path_t          fsal_path ;
  fsal_handle_t        dir_handle ;
  fsal_attrib_list_t   dir_attr ;
  fsal_dir_t           dir_descriptor ;
  fsal_dir_t           subdir_descriptor ;
  fsal_dirent_t        dirent[NB_DIRENT_CLEAN] ;
  fsal_dirent_t        subdirent[NB_DIRENT_CLEAN] ;
  fsal_cookie_t        end_cookie ;
  fsal_cookie_t        subend_cookie ;
  fsal_count_t         nb_entries ;
  fsal_count_t         subnb_entries ;
  fsal_count_t         nb_count ;
  fsal_count_t         subnb_count ;
  fsal_boolean_t       eod = FALSE ; 
  fsal_boolean_t       subeod = FALSE ; 
  fsal_status.major = ERR_FSAL_NO_ERROR ;
  fsal_status.minor = 0 ;


  fsal_status = FSAL_str2path(  mfsl_param.pre_create_obj_dir, MAXPATHLEN, &fsal_path ) ;
  if( FSAL_IS_ERROR( fsal_status ) )
   {
      DisplayLog( "Impossible to convert path %s", mfsl_param.pre_create_obj_dir ) ;
      exit( 1 ) ; 
   }

  fsal_status = FSAL_lookupPath( &fsal_path, pcontext, &dir_handle, &dir_attr ) ;
  if( FSAL_IS_ERROR( fsal_status ) )
   {
      DisplayLog( "Impossible to lookup directory %s to be used to store precreated objects: status=(%u,%u)",
  		  mfsl_param.pre_create_obj_dir, fsal_status.major, fsal_status.minor ) ;
      exit( 1 ) ; 
   }

  while( eod == FALSE )
   {
     fsal_status = FSAL_opendir( &dir_handle, 
                                 pcontext, 
                                 &dir_descriptor, 
                                 &dir_attr ) ;
     if( FSAL_IS_ERROR( fsal_status ) )
      {
         DisplayLog( "Impossible to opendir directory %s to be used to store precreated objects: status=(%u,%u)",
      		     mfsl_param.pre_create_obj_dir, fsal_status.major, fsal_status.minor ) ;
         exit( 1 ) ; 
      }


     fsal_status =  FSAL_readdir( &dir_descriptor,
     		                  FSAL_READDIR_FROM_BEGINNING, 
  			          FSAL_ATTRS_MANDATORY,
  			          NB_DIRENT_CLEAN * sizeof( fsal_dirent_t ), 			        	
			          dirent,
			          &end_cookie,
			          &nb_entries,
			          &eod ) ;
     if( FSAL_IS_ERROR( fsal_status ) )
      {  
         DisplayLog( "Impossible to readdir directory %s to be used to store precreated objects: status=(%u,%u)",
  	   	     mfsl_param.pre_create_obj_dir, fsal_status.major, fsal_status.minor ) ;
         exit( 1 ) ; 
      }

     fsal_status = FSAL_closedir( &dir_descriptor ) ;
     if( FSAL_IS_ERROR( fsal_status ) )
      {
         DisplayLog( "Impossible to closedir directory %s to be used to store precreated objects: status=(%u,%u)",
  		     mfsl_param.pre_create_obj_dir, fsal_status.major, fsal_status.minor ) ;
         exit( 1 ) ; 
      }

     for( nb_count = 0 ; nb_count < nb_entries ; nb_count ++ )
      {
         fsal_status =  FSAL_unlink( &dir_handle,
				     &dirent[nb_count].name,
				     pcontext, 
				     &dir_attr ) ;
         if( FSAL_IS_ERROR( fsal_status ) )
          {
             if( fsal_status.major != ERR_FSAL_NOTEMPTY )
              {
#ifdef _USE_PROXY
		if( fsal_status.minor == NFS4ERR_GRACE )
		    DisplayLog( "The remote server is within grace period. Wait for grace period to end and retry" ) ;
                else
#endif
                DisplayLog( "Impossible to unlink %s/%s status=(%u,%u)",
  		             mfsl_param.pre_create_obj_dir, dirent[nb_count].name.name, fsal_status.major, fsal_status.minor ) ;
                exit( 1 ) ; 
              }
            else
              {
                while( subeod == FALSE ) 
                 {
		    /* non empty directory, cleanup it too.. */
    		    fsal_status = FSAL_opendir( &dirent[nb_count].handle,
                    	                        pcontext, 
                           	                &subdir_descriptor, 
                                 	        &dir_attr ) ;
   	           if( FSAL_IS_ERROR( fsal_status ) )
      		    {
         		DisplayLog( "Impossible to opendir directory %s/%s to be used to store precreated objects: status=(%u,%u)",
      		   	  	mfsl_param.pre_create_obj_dir, dirent[nb_count].name.name, fsal_status.major, fsal_status.minor ) ;
         		exit( 1 ) ; 
    	            }


     	           fsal_status =  FSAL_readdir( &subdir_descriptor,
     	 	                                FSAL_READDIR_FROM_BEGINNING, 
  			                        FSAL_ATTRS_MANDATORY,
  			                        NB_DIRENT_CLEAN * sizeof( fsal_dirent_t ), 			        	
			          	        subdirent,
			                        &subend_cookie,
			          	        &subnb_entries,
			          	        &subeod ) ;
     	           if( FSAL_IS_ERROR( fsal_status ) )
      		    {  
        	      DisplayLog( "Impossible to readdir directory %s/%s to be used to store precreated objects: status=(%u,%u)",
  	   	     	          mfsl_param.pre_create_obj_dir, dirent[nb_count].name.name,fsal_status.major, fsal_status.minor ) ;
         	      exit( 1 ) ; 
      		    }
 
	           fsal_status = FSAL_closedir( &subdir_descriptor ) ;
     	           if( FSAL_IS_ERROR( fsal_status ) )
     		     {
         	        DisplayLog( "Impossible to closedir directory %s to be used to store precreated objects: status=(%u,%u)",
  		                    mfsl_param.pre_create_obj_dir, dirent[nb_count].name.name, fsal_status.major, fsal_status.minor ) ;
         	        exit( 1 ) ; 
      		     }

     		    for( subnb_count = 0 ; subnb_count < subnb_entries ; subnb_count ++ )
      		    {
         	      fsal_status =  FSAL_unlink( &dirent[nb_count].handle, 
			 		          &subdirent[subnb_count].name,
				     	          pcontext, 
				     	          &dir_attr ) ;
         	      if( FSAL_IS_ERROR( fsal_status ) )
         	      {
             	         if( fsal_status.major != ERR_FSAL_NOTEMPTY )
             	          {
#ifdef _USE_PROXY
                 	    if( fsal_status.minor == NFS4ERR_GRACE )
		              DisplayLog( "The remote server is within grace period. Wait for grace period to end and retry" ) ;
                	    else
#endif
                	    DisplayLog( "Impossible to unlink %s/%s/%s status=(%u,%u)",
  		                         mfsl_param.pre_create_obj_dir, dirent[nb_count].name.name, subdirent[subnb_count], 
				         fsal_status.major, fsal_status.minor ) ;
                            exit( 1 ) ; 
                          }
		      }
                     } /* for */
		 } /* while (subeod ) */ 
		subeod = FALSE ;
              } /* else */
          } /* if */
      } /* for */

   } /* while( eod ) */

  return fsal_status ;
} /* mfsl_async_init_clean_precreated_object */

/**
 * 
 * mfsl_async_init_precreated_directories: allocate pre-created directories for asynchronous create.
 *
 * Allocate pre-created directories for asynchronous create.
 *
 * @param pcontext    [INOUT] pointer to FSAL context to be used 
 * @param pool_dirs   [INOUT] pointer to MFSL precreated entries to be created
 *
 * @return a FSAL status
 *
 */
fsal_status_t mfsl_async_init_precreated_directories( fsal_op_context_t         * pcontext,
						      mfsl_precreated_object_t  * pool_dirs )
{
  unsigned int                   i = 0 ;
  char                           newdirpath[MAXNAMLEN] ;
  pthread_t                      me = pthread_self() ;
  unsigned int                   pid = (unsigned int)getpid() ;
  fsal_status_t                  fsal_status ;
  fsal_path_t                    fsal_path ;
  fsal_name_t                    fsal_name ;
  mfsl_precreated_object_t     * pprecreated ;
  fsal_attrib_list_t             dir_attr ;
  static unsigned int            counter = 0 ;
  mfsl_precreated_object_t     * piter = NULL ;

  fsal_status.major = ERR_FSAL_NO_ERROR ;
  fsal_status.minor = 0 ;

  fsal_status = FSAL_str2path(  mfsl_param.pre_create_obj_dir, MAXPATHLEN, &fsal_path ) ;
  if( FSAL_IS_ERROR( fsal_status ) )
   {
      DisplayLog( "Impossible to convert path %s", mfsl_param.pre_create_obj_dir ) ;
      exit( 1 ) ; 
   }

  dir_attr.asked_attributes = FSAL_ATTRS_POSIX ;
  dir_attr.supported_attributes = FSAL_ATTRS_POSIX ;
  
  if( dir_handle_set == 0 )
    {
      fsal_status = FSAL_lookupPath( &fsal_path, pcontext, &dir_handle_precreate, &dir_attr ) ;
      if( FSAL_IS_ERROR( fsal_status ) )
       {
        DisplayLog( "Impossible to lookup directory %s to be used to store precreated objects: status=(%u,%u)",
  		    mfsl_param.pre_create_obj_dir, fsal_status.major, fsal_status.minor ) ;
        exit( 1 ) ; 
       }

      dir_handle_set = 1 ;
    }

  for( piter = pool_dirs ; piter != NULL ; piter = piter->next_alloc )
   {
      if( piter->inited != 0 )
        continue ;

      snprintf( newdirpath, MAXNAMLEN, "dir.%u.%lu.%u", pid, me, counter++ ) ;
      fsal_status = FSAL_str2name( newdirpath, MAXNAMLEN, &fsal_name ) ;
      if( FSAL_IS_ERROR( fsal_status ) ) 
       {
          DisplayLog( "Impossible to convert name %s", newdirpath ) ;
          exit( 1 ) ; 
       }
 
      pprecreated = piter ;
      pprecreated->name = fsal_name ;
      pprecreated->attr.asked_attributes = FSAL_ATTRS_POSIX ;
      pprecreated->attr.supported_attributes = FSAL_ATTRS_POSIX ;
      pthread_mutex_init( &pprecreated->mobject.lock, NULL ) ;

      piter->inited = 1 ;

      fsal_status = FSAL_mkdir( &dir_handle_precreate, 
				&fsal_name,
				pcontext,
				0777, 
			        &(pprecreated->mobject.handle), 
				&(pprecreated->attr) ) ;
      if( FSAL_IS_ERROR( fsal_status ) ) 
       {
          DisplayLog( "Impossible to mkdir %s/%s, status=(%u,%u)", 
		      mfsl_param.pre_create_obj_dir, newdirpath, fsal_status.major, fsal_status.minor ) ;
          exit( 1 ) ; 
       }
   }

 return fsal_status ;
} /* mfsl_async_init_precreated_directories */

/**
 * 
 * mfsl_async_init_precreated_files: allocate pre-created files for asynchronous create.
 *
 * Allocate pre-created files for asynchronous create.
 *
 * @param pcontext      [INOUT] pointer to FSAL context to be used 
 * @param pool_file     [INOUT] pointer to MFSL precreated entries to be created
 *
 * @return a FSAL status
 *
 */
fsal_status_t mfsl_async_init_precreated_files( fsal_op_context_t         * pcontext, 
						mfsl_precreated_object_t  * pool_files )
{
  unsigned int                   i = 0 ;
  char                           newdirpath[MAXNAMLEN] ;
  pthread_t                      me = pthread_self() ;
  unsigned int                   pid = (unsigned int)getpid() ;
  fsal_status_t                  fsal_status ;
  fsal_path_t                    fsal_path ;
  fsal_name_t                    fsal_name ;
  mfsl_precreated_object_t     * pprecreated ;
  fsal_attrib_list_t             dir_attr ;
  fsal_attrib_list_t             obj_attr ;
  mfsl_precreated_object_t     * piter = NULL ;
  static unsigned int            counter = 0 ;

  fsal_status.major = ERR_FSAL_NO_ERROR ;
  fsal_status.minor = 0 ;

  fsal_status = FSAL_str2path(  mfsl_param.pre_create_obj_dir, MAXPATHLEN, &fsal_path ) ;
  if( FSAL_IS_ERROR( fsal_status ) )
   {
      DisplayLog( "Impossible to convert path %s", mfsl_param.pre_create_obj_dir ) ;
      exit( 1 ) ; 
   }

  dir_attr.asked_attributes = FSAL_ATTRS_POSIX ;
  dir_attr.supported_attributes = FSAL_ATTRS_POSIX ;
  if( dir_handle_set == 0 )
    {
      fsal_status = FSAL_lookupPath( &fsal_path, pcontext, &dir_handle_precreate, &dir_attr ) ;
      if( FSAL_IS_ERROR( fsal_status ) )
       {
        DisplayLog( "Impossible to lookup directory %s to be used to store precreated objects: status=(%u,%u)",
  		    mfsl_param.pre_create_obj_dir, fsal_status.major, fsal_status.minor ) ;
        exit( 1 ) ; 
       }

      dir_handle_set = 1 ;
    }

  for( piter = pool_files ; piter != NULL ; piter = piter->next_alloc ) 
   {
      if( piter->inited != 0 )
	continue ;

      snprintf( newdirpath, MAXNAMLEN, "file.%u.%lu.%u", pid, me, counter++ ) ;
      fsal_status = FSAL_str2name( newdirpath, MAXNAMLEN, &fsal_name ) ;
      if( FSAL_IS_ERROR( fsal_status ) ) 
       {
          DisplayLog( "Impossible to convert name %s", newdirpath ) ;
          exit( 1 ) ; 
       }

      pprecreated = piter ;
      pprecreated->name = fsal_name ;
      pprecreated->attr.asked_attributes = FSAL_ATTRS_POSIX ;
      pprecreated->attr.supported_attributes = FSAL_ATTRS_POSIX ;
      pthread_mutex_init( &pprecreated->mobject.lock, NULL ) ;

      piter->inited = 1 ;

      fsal_status = FSAL_create( &dir_handle_precreate, 
			 	 &fsal_name,
				 pcontext,
				 0777, 
			         &(pprecreated->mobject.handle), 
				 &(pprecreated->attr) ) ;
      if( FSAL_IS_ERROR( fsal_status ) ) 
       {
          DisplayLog( "Impossible to create %s/%s, status=(%u,%u)", 
		      mfsl_param.pre_create_obj_dir, newdirpath, fsal_status.major, fsal_status.minor ) ;
          exit( 1 ) ; 
       }
   }

 return fsal_status ;
} /* mfsl_async_init_precreated_files */

/**
 * 
 * MFSL_PrepareContext: Prepares a MFSL context for a thead.
 *
 * Prepares a MFSL context for a thread.
 *
 * @param pcontext      [INOUT] pointer to MFSL context to be used 
 *
 * @return a FSAL status
 *
 */
fsal_status_t MFSL_PrepareContext( fsal_op_context_t  * pcontext )
{
   return mfsl_async_init_clean_precreated_objects( pcontext ) ;
} /* MFSL_PrepareContext */
 

/**
 * 
 * MFSL_GetContext: Creates a MFSL context for a thead.
 *
 * Creates a MFSL context for a thread.
 *
 * @param pcontext      [INOUT] pointer to MFSL context to be used 
 * @param pfsal_context [INOUT] pointer to FSAL context to be used
 *
 * @return a FSAL status
 *
 */
fsal_status_t MFSL_GetContext( mfsl_context_t     * pcontext,
                               fsal_op_context_t  * pfsal_context  ) 
{ 
  fsal_status_t          status ;

  status.major = ERR_FSAL_NO_ERROR ;
  status.minor = 0 ;

  if( pthread_mutex_init( &pcontext->lock, NULL ) != 0 )
     MFSL_return( ERR_FSAL_SERVERFAULT, errno ) ;

  pcontext->synclet_index = 0 ; /* only one synclet for now */

  STUFF_PREALLOC( pcontext->pool_async_op,
                  mfsl_param.nb_pre_async_op_desc,
		  mfsl_async_op_desc_t,
		  next_alloc ) ;
 
  STUFF_PREALLOC( pcontext->pool_spec_data,
		  mfsl_param.nb_pre_async_op_desc,
		  mfsl_object_specific_data_t,
		  next_alloc ) ;

  pcontext->log_outputs = log_outputs ;

  /* Preallocate files and dirs for this thread */
  status = MFSL_RefreshContext( pcontext, pfsal_context ) ;

  return status ;
} /* MFSL_GetContext */

 /**
 * 
 * MFSL_GetSyncletContext: Creates a MFSL context for a synclet.
 *
 * Creates a MFSL context for a synclet.
 *
 * @param pcontext      [INOUT] pointer to MFSL context to be used 
 * @param pfsal_context [INOUT] pointer to FSAL context to be used
 *
 * @return a FSAL status
 *
 */
fsal_status_t MFSL_ASYNC_GetSyncletContext( mfsl_synclet_context_t  * pcontext,
                                            fsal_op_context_t       * pfsal_context  ) 
{
   fsal_status_t          status ;

   status.major = ERR_FSAL_NO_ERROR ;
   status.minor = 0 ;

  if( pthread_mutex_init( &pcontext->lock, NULL ) != 0 )
     MFSL_return( ERR_FSAL_SERVERFAULT, errno ) ;

   return status;
}

/**
 * 
 * MFSL_ASYNC_RefreshContextDirs: Refreshes the pool of pre-allocated directories for a MFSL context.
 *
 * Refreshes the pool of pre-allocated directories for a MFSL conte
 *
 * @param pcontext      [INOUT] pointer to MFSL context to be used 
 * @param pfsal_context [INOUT] pointer to FSAL context to be used
 *
 * @return a FSAL status
 *
 */
fsal_status_t MFSL_ASYNC_RefreshContextDirs( mfsl_context_t     * pcontext,
                                             fsal_op_context_t  * pfsal_context  ) 
{
   fsal_status_t          status ;
  
   status.major = ERR_FSAL_NO_ERROR ;
   status.minor = 0 ;

   if( pcontext->pool_dirs == NULL )
    {
     STUFF_PREALLOC_CONSTRUCT( pcontext->pool_dirs,
  		               mfsl_param.nb_pre_create_dirs,
		               mfsl_precreated_object_t,
		               next_alloc,
 			       constructor_preacreated_entries ) ;
     pcontext->avail_pool_dirs = mfsl_param.nb_pre_create_dirs ;

     status =  mfsl_async_init_precreated_directories( pfsal_context, pcontext->pool_dirs ) ;
     if( FSAL_IS_ERROR( status ) ) 
      return status ;
    }

   return status;
} /* MFSL_ASYNC_RefreshContextDirs */

/**
 * 
 * MFSL_ASYNC_RefreshContextDirs: Refreshes the pool of pre-allocated files for a MFSL context.
 *
 * Refreshes the pool of pre-allocated files for a MFSL conte
 *
 * Creates a MFSL context for a thread.
 *
 * @param pcontext      [INOUT] pointer to MFSL context to be used 
 * @param pfsal_context [INOUT] pointer to FSAL context to be used
 *
 * @return a FSAL status
 *
 */
fsal_status_t MFSL_ASYNC_RefreshContextFiles( mfsl_context_t     * pcontext,
                                              fsal_op_context_t  * pfsal_context  ) 
{
   fsal_status_t          status ;
  
   status.major = ERR_FSAL_NO_ERROR ;
   status.minor = 0 ;

   if( pcontext->pool_files == NULL ) 
    {
     STUFF_PREALLOC_CONSTRUCT( pcontext->pool_files,
  		               mfsl_param.nb_pre_create_files,
		               mfsl_precreated_object_t,
		               next_alloc,
			       constructor_preacreated_entries ) ;
     pcontext->avail_pool_files = mfsl_param.nb_pre_create_files ;
     
     status =  mfsl_async_init_precreated_files( pfsal_context, pcontext->pool_files ) ;
     if( FSAL_IS_ERROR( status ) ) 
       return status ;
    }

   return status;
} /* MFSL_ASYNC_RefreshtContextFiles */

/**
 * 
 * MFSL_RefreshContext: Refreshes a MFSL context for a thead.
 *
 * Refreshes a MFSL context for a thread.
 *
 * @param pcontext      [INOUT] pointer to MFSL context to be used 
 * @param pfsal_context [INOUT] pointer to FSAL context to be used
 *
 * @return a FSAL status
 *
 */
fsal_status_t MFSL_RefreshContext( mfsl_context_t     * pcontext,
                                   fsal_op_context_t  * pfsal_context  ) 
{
   fsal_status_t               status ;
   fsal_export_context_t       fsal_export_context ;
   fsal_op_context_t           fsal_context ;
 
   status.major = ERR_FSAL_NO_ERROR ;
   status.minor = 0 ;

   if( !pcontext->pool_dirs || !pcontext->pool_files )
    {
       status = FSAL_BuildExportContext( &fsal_export_context,
                                         NULL,
                                         NULL ) ;
       if( FSAL_IS_ERROR( status ) ) 
         return status ; 

       status = FSAL_GetClientContext( pfsal_context,
                                       &fsal_export_context,
                                       0,
                                       0,
                                       NULL,
                                       0);
       if( FSAL_IS_ERROR( status ) ) 
         return status ; 
     } /*  if( !pcontext->pool_dirs || !pcontext->pool_files ) */


   if( pcontext->pool_dirs == NULL )
    {
      status = MFSL_ASYNC_RefreshContextDirs( pcontext, pfsal_context ) ;
      if( FSAL_IS_ERROR( status ) ) 
         return status ;
    }

   if( pcontext->pool_files == NULL ) 
    {
      status = MFSL_ASYNC_RefreshContextFiles( pcontext, pfsal_context ) ;
      if( FSAL_IS_ERROR( status ) ) 
         return status ;
    }
  
  return status ;
} /* MFSL_ASYNC_RefreshContext */

/**
 * 
 * MFSL_ASYNC_RefreshSyncletContext: Refreshes a MFSL context for a synclet.
 *
 * Refreshes a MFSL context for a synclet.
 *
 * @param pcontext      [INOUT] pointer to MFSL context to be used 
 * @param pfsal_context [INOUT] pointer to FSAL context to be used
 *
 * @return a FSAL status
 *
 */
fsal_status_t MFSL_ASYNC_RefreshSyncletContext( mfsl_synclet_context_t  * pcontext,
                                                fsal_op_context_t       * pfsal_context  ) 
{
   fsal_status_t          status ;
  
   status.major = ERR_FSAL_NO_ERROR ;
   status.minor = 0 ;

  return status ;
} /* MFSL_ASYNC_RefreshSyncletContext */


/**
 * 
 * MFSL_ASYNC_is_synced: returns TRUE if the object is synced, FALSE is asynchronous.
 *
 * Returns TRUE if the object is synced, FALSE is asynchronous.
 *
 * @param mobject      [IN] pointer to MFSL object to be tested
 *
 * @return  TRUE if the object is synced, FALSE is asynchronous. 
 *
 */
int MFSL_ASYNC_is_synced( mfsl_object_t * mobject )
{
   if( mobject == NULL ) 
	return FALSE ;

   if( mobject->health == MFSL_ASYNC_SYNCHRONOUS )
 	return TRUE ;

   /* in all other case, returns FALSE */
   return FALSE ;
} /*  MFSL_ASYNC_is_synced */

#endif /* ! _USE_SWIG */



/******************************************************
 *              Common Filesystem calls.
 ******************************************************/

fsal_status_t MFSL_lookupPath (
    fsal_path_t           * p_path,            /* IN */
    fsal_op_context_t     * p_context,         /* IN */
    mfsl_context_t        * p_mfsl_context,    /* IN */
    mfsl_object_t         * object_handle,     /* OUT */
    fsal_attrib_list_t    * object_attributes  /* [ IN/OUT ] */
)
{
  fsal_status_t fsal_status ;

  fsal_status = FSAL_lookupPath( p_path,            
	  		         p_context,        
    			         &object_handle->handle,    
   			         object_attributes ) ;
  return fsal_status ;
} /* MFSL_lookupPath */



fsal_status_t MFSL_lookupJunction (
    mfsl_object_t         * p_junction_handle,   /* IN */
    fsal_op_context_t     * p_context,           /* IN */
    mfsl_context_t        * p_mfsl_context,      /* IN */
    mfsl_object_t         * p_fsoot_handle,      /* OUT */
    fsal_attrib_list_t    * p_fsroot_attributes  /* [ IN/OUT ] */
)
{
    return FSAL_lookupJunction( &p_junction_handle->handle,
                                p_context,
				&p_fsoot_handle->handle,
                                p_fsroot_attributes ) ;
} /* MFSL_lookupJunction */
				                                  



fsal_status_t MFSL_access(
    mfsl_object_t              * object_handle,      /* IN */
    fsal_op_context_t          * p_context,          /* IN */
    mfsl_context_t             * p_mfsl_context,     /* IN */
    fsal_accessflags_t           access_type,        /* IN */
    fsal_attrib_list_t         * object_attributes   /* [ IN/OUT ] */
)
{
  fsal_status_t fsal_status ;

  P( object_handle->lock ) ;
  fsal_status = FSAL_access(  &object_handle->handle,
			      p_context,
			      access_type,
			      object_attributes ) ;
  V( object_handle->lock ) ;

   return fsal_status ;
} /* MFSL_access */


fsal_status_t MFSL_opendir(
    mfsl_object_t             * dir_handle,           /* IN */
    fsal_op_context_t         * p_context,            /* IN */
    mfsl_context_t            * p_mfsl_context,       /* IN */
    fsal_dir_t                * dir_descriptor,       /* OUT */
    fsal_attrib_list_t        * dir_attributes        /* [ IN/OUT ] */
)
{
  fsal_status_t fsal_status ;

  P( dir_handle->lock ) ;
  fsal_status = FSAL_opendir( &dir_handle->handle,
                              p_context,
                              dir_descriptor,
		              dir_attributes ) ;
  V( dir_handle->lock ) ;

  return fsal_status ;
} /* MFSL_opendir */

fsal_status_t MFSL_readdir(
    fsal_dir_t            * dir_descriptor,     /* IN */
    fsal_cookie_t         start_position ,      /* IN */
    fsal_attrib_mask_t    get_attr_mask,        /* IN */
    fsal_mdsize_t         buffersize,           /* IN */
    fsal_dirent_t         * pdirent,            /* OUT */
    fsal_cookie_t         * end_position,       /* OUT */
    fsal_count_t          * nb_entries,         /* OUT */
    fsal_boolean_t        * end_of_dir,         /* OUT */
    mfsl_context_t        * p_mfsl_context      /* IN */
)
{
  fsal_status_t fsal_status ;

    return  FSAL_readdir( dir_descriptor,
       	                  start_position,
	        	  get_attr_mask,
	       		  buffersize,
			  pdirent,
			  end_position,
			  nb_entries,
			  end_of_dir ) ;

} /* MFSL_readdir */


fsal_status_t MFSL_closedir(
    fsal_dir_t      * dir_descriptor,    /* IN */
    mfsl_context_t  * p_mfsl_context     /* IN */
)
{
  fsal_status_t fsal_status ;

   return FSAL_closedir( dir_descriptor ) ;
} /* FSAL_closedir */

fsal_status_t MFSL_open(
    mfsl_object_t         * filehandle,             /* IN */
    fsal_op_context_t     * p_context,              /* IN */
    mfsl_context_t        * p_mfsl_context,         /* IN */
    fsal_openflags_t      openflags,                /* IN */
    fsal_file_t           * file_descriptor,        /* OUT */
    fsal_attrib_list_t    * file_attributes         /* [ IN/OUT ] */
)
{
  fsal_status_t fsal_status ;

  fsal_status =  FSAL_open( 	&filehandle->handle,
		 		p_context,
				openflags, 
				file_descriptor,
				file_attributes ) ;
  return fsal_status ;
} /* MFSL_open */

fsal_status_t MFSL_open_by_fileid(
    mfsl_object_t         * filehandle,             /* IN */
    fsal_u64_t              fileid,                 /* IN */
    fsal_op_context_t     * p_context,              /* IN */
    mfsl_context_t        * p_mfsl_context,         /* IN */
    fsal_openflags_t        openflags,              /* IN */
    fsal_file_t           * file_descriptor,        /* OUT */
    fsal_attrib_list_t    * file_attributes         /* [ IN/OUT ] */ ) 
{
    return FSAL_open_by_fileid( &filehandle->handle,
				fileid,
				p_context,
				openflags,
				file_descriptor,
				file_attributes ) ;
} /* MFSL_open_by_fileid */

fsal_status_t MFSL_read(
    fsal_file_t           * file_descriptor,         /*  IN  */
    fsal_seek_t           * seek_descriptor,         /* [IN] */
    fsal_size_t             buffer_size,             /*  IN  */
    caddr_t                 buffer,                  /* OUT  */
    fsal_size_t           * read_amount,             /* OUT  */
    fsal_boolean_t        * end_of_file,             /* OUT  */
    mfsl_context_t        * p_mfsl_context           /* IN */
)
{
    return FSAL_read(	file_descriptor,
			seek_descriptor,
			buffer_size,
			buffer,
			read_amount,
			end_of_file ) ;
} /* MFSL_read */

fsal_status_t MFSL_write(
    fsal_file_t        * file_descriptor,     /* IN */
    fsal_seek_t        * seek_descriptor,     /* IN */
    fsal_size_t         buffer_size,          /* IN */
    caddr_t             buffer,               /* IN */
    fsal_size_t        * write_amount,        /* OUT */
    mfsl_context_t     * p_mfsl_context       /* IN */
)
{
   return FSAL_write(	file_descriptor,
			seek_descriptor,
			buffer_size,
			buffer,
			write_amount ) ;
} /* MFSL_write */

fsal_status_t MFSL_close(
    fsal_file_t        * file_descriptor, /* IN */
    mfsl_context_t     * p_mfsl_context                 /* IN */
)
{
    return FSAL_close( file_descriptor ) ;
} /* MFSL_close */

fsal_status_t MFSL_close_by_fileid(
    fsal_file_t        * file_descriptor /* IN */,
    fsal_u64_t           fileid, 
    mfsl_context_t     * p_mfsl_context )                /* IN */
{
   return FSAL_close_by_fileid( file_descriptor,
				fileid ) ;
} /* MFSL_close_by_fileid */


fsal_status_t MFSL_readlink(
    mfsl_object_t         * linkhandle,     /* IN */
    fsal_op_context_t     * p_context,      /* IN */
    mfsl_context_t        * p_mfsl_context, /* IN */
    fsal_path_t           * p_link_content, /* OUT */
    fsal_attrib_list_t    * link_attributes /* [ IN/OUT ] */
)
{
   fsal_status_t fsal_status ;

   P( linkhandle->lock ) ;
   fsal_status = FSAL_readlink( &linkhandle->handle,
		 	        p_context,
                                p_link_content,
                                link_attributes ) ;
   V( linkhandle->lock ) ;

   return fsal_status ;
} /* MFSL_readlink */

fsal_status_t MFSL_mknode(
    mfsl_object_t             * parentdir_handle,       /* IN */
    fsal_name_t               * p_node_name,            /* IN */
    fsal_op_context_t         * p_context,              /* IN */
    mfsl_context_t            * p_mfsl_context,         /* IN */
    fsal_accessmode_t           accessmode,             /* IN */
    fsal_nodetype_t             nodetype,               /* IN */
    fsal_dev_t                * dev,                    /* IN */
    mfsl_object_t             * p_object_handle,        /* OUT */
    fsal_attrib_list_t        * node_attributes         /* [ IN/OUT ] */
)
{
    return FSAL_mknode(	&parentdir_handle->handle,
			p_node_name,
			p_context,
			accessmode, 
			nodetype,
			dev,
			&p_object_handle->handle,
			node_attributes ) ;
} /* MFSL_mknode */

fsal_status_t  MFSL_rcp(
    mfsl_object_t           *   filehandle,     /* IN */
    fsal_op_context_t       *   p_context,      /* IN */
    mfsl_context_t          *   p_mfsl_context, /* IN */
    fsal_path_t             *   p_local_path,   /* IN */
    fsal_rcpflag_t              transfer_opt    /* IN */
)
{
   fsal_status_t fsal_status ;

   P( filehandle->lock ) ;
   fsal_status = FSAL_rcp( &filehandle->handle,
		           p_context,
		           p_local_path,
		           transfer_opt ) ;	
   V( filehandle->lock ) ;
   
   return fsal_status ;
} /* MFSL_rcp */

fsal_status_t  MFSL_rcp_by_fileid(
    mfsl_object_t             * filehandle,     /* IN */
    fsal_u64_t                  fileid,         /* IN */
    fsal_op_context_t         * p_context,      /* IN */
    mfsl_context_t            * p_mfsl_context, /* IN */
    fsal_path_t               * p_local_path,   /* IN */
    fsal_rcpflag_t            transfer_opt      /* IN */
)
{
   fsal_status_t fsal_status ;

   P( filehandle->lock ) ;
   fsal_status = FSAL_rcp_by_fileid( 	&filehandle->handle,
					fileid,
					p_context,
					p_local_path,
					transfer_opt ) ;
   V( filehandle->lock ) ;
   
   return fsal_status ;
} /* MFSL_rcp_by_fileid */


/* To be called before exiting */
fsal_status_t  MFSL_terminate( void ) 
{
   fsal_status_t status ;
   
   end_of_mfsl = TRUE ; 
 
   status.major = ERR_FSAL_NO_ERROR ;
   status.minor = 0 ;

   return status ;

} /* MFSL_terminate */

#ifndef _USE_SWIG

/******************************************************
 *                FSAL locks management.
 ******************************************************/

fsal_status_t  MFSL_lock(
    mfsl_object_t           * objecthandle,     /* IN */
    fsal_op_context_t       * p_context,        /* IN */
    mfsl_context_t          * p_mfsl_context,   /* IN */
    fsal_lockparam_t        * lock_info,        /* IN */
    fsal_lockdesc_t         * lock_descriptor   /* OUT */
)
{
    return FSAL_lock( &objecthandle->handle,
		      p_context,
 		      lock_info,
		      lock_descriptor ) ;	
} /* MFSL_lock */

fsal_status_t  MFSL_changelock(
    fsal_lockdesc_t      * lock_descriptor,      /* IN / OUT */
    fsal_lockparam_t     * lock_info,            /* IN */
    mfsl_context_t       * p_mfsl_context          /* IN */
)
{
   return FSAL_changelock( lock_descriptor,
			   lock_info ) ;
} /* MFSL_changelock */

fsal_status_t  MFSL_unlock(
    fsal_lockdesc_t * lock_descriptor,        /* IN/OUT */
    mfsl_context_t  * p_mfsl_context          /* IN */
)
{
    return FSAL_unlock( lock_descriptor );
} /* MFSL_unlock */



#endif /* ! _USE_SWIG */


