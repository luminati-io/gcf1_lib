#ifndef _GCF1_H_
#define _GCF1_H_

/*
 * C language interface to the GCFSDK library
 * Copyright (C) 2009-2021 NetSTAR Inc. All rights reserved.
 * See the LICENSE for redistribution information.
 */


#ifdef __cplusplus
#ifndef WIN32
extern "C"
{
#else
#if defined(LIBGCF1_EXPORTS)
#define LIBGCF1_API __declspec(dllexport)
#else
#ifndef LIBGCF1_STATICLINK
#define LIBGCF1_API __declspec(dllimport)
#else
#define LIBGCF1_API
#endif
#endif
#endif

#endif

#ifndef LIBGCF1_API
#define LIBGCF1_API
#endif

#ifndef LIBGCF_HCTC
#define LIBGCF_HCTC
	typedef void * HCTC;
#endif

#ifndef LIBGCF_HCTM
#define LIBGCF_HCTM
	typedef void * HCTM;
#endif

#ifndef LIBGCF_HCTE
#define LIBGCF_HCTE
	typedef void * HCTE;
#endif

#ifndef LIBGCF_IAB_CATEGORY_RESULT3_S
#define LIBGCF_IAB_CATEGORY_RESULT3_S
	struct iab_category_result3_s
	{
		int size;
		int count;
		int iab_t1_category[16];
		int iab_t2_category[16];
		
		int level[16];
		unsigned int flag[16];

		int iab_t3_category[16];
		int iab_t4_category[16];
		
		int iqg_content_rating[16];
		int iqg_non_standard[16];
		int iqg_illegal_content[16];
		int iqg_brand_safe[16];
	};
#endif

#ifndef LIBGCF_CATEGORY_RESULT_EX1_S
#define LIBGCF_CATEGORY_RESULT_EX1_S
	struct category_result_ex1_s
	{
		int count;
		int category[10];
		int rate[10];
	};
#endif

#ifndef LIBGCF_CATEGORY_INFO_S
#define LIBGCF_CATEGORY_INFO_S
	struct category_info_s
	{
		char name[255];
		char description[1024];
		char extra_info[1024];
	};
#endif

#ifndef LIBGCF_LIST_ITEM_S
#define LIBGCF_LIST_ITEM_S
	struct list_item_s
	{
		char **columns;
	};
#endif
#ifndef LIBGCF_LIST_RESULT_S
#define LIBGCF_LIST_RESULT_S
	struct list_result_s
	{
		int size;
		int list_count;
		int column_count;
		struct list_item_s **items;
	};
#endif

#ifndef LIBGCF_CATEGORIZE_RESULT_S
#define LIBGCF_CATEGORIZE_RESULT_S
	struct categorize_result_s
	{
		int size;
		int count;
		int type[16];
		int category[16];
		int category_secondary[16];
		int category_security[16];
		unsigned int flag[16];
		int reputation[16];
		int age_rating[16];
		int country_code_length[16];
		char country_code[16][256];
		int location_string_length[16];
		char location_string[16][256];
		int category_group_count[16];
		int category_group[16][16];
		int volume_index[16];
	};
#endif


	extern LIBGCF1_API int gcf1_check_begin(const char *config_path, int option, HCTC *handle);
	extern LIBGCF1_API int gcf1_check_end(HCTC handle, int option);
	extern LIBGCF1_API int gcf1_check_reload(HCTC handle, int option);
	extern LIBGCF1_API int gcf1_check_dbversion(HCTC handle, char *version_string, int *version_length);
	extern LIBGCF1_API int gcf1_check_iab3(HCTC handle, const char *url, int option , struct iab_category_result3_s *category_result);

	extern LIBGCF1_API int gcf1_dbmng_begin(const char*config_path, int option, HCTM *handle);
	extern LIBGCF1_API int gcf1_dbmng_end(HCTM handle, int option);
	extern LIBGCF1_API int gcf1_dbmng_download(HCTM handle, int option);
	extern LIBGCF1_API int gcf1_dbmng_update(HCTM handle, int option);
	extern LIBGCF1_API int gcf1_dbmng_clear_cachedb(HCTM handle, int option, const char *domain);

	extern LIBGCF1_API int gcf1_dbmng_open_usrdb(HCTM handle, int option);
	extern LIBGCF1_API int gcf1_dbmng_close_usrdb(HCTM handle, int option);
	extern LIBGCF1_API int gcf1_dbmng_add_usrdb(HCTM handle, int catid, int group, char *url, int method);

	extern LIBGCF1_API int gcf1_dbmng_open_usrcategory(HCTM handle, int option);
	extern LIBGCF1_API int gcf1_dbmng_close_usrcategory(HCTM handle, int option);
	extern LIBGCF1_API int gcf1_dbmng_add_usrcategory(HCTM handle, int option, const char *file_type, const char *category_number, const char *category_name, const char *description, const char *extra_info);

	extern LIBGCF1_API int gcf1_dbmng_extra_download(HCTM handle, int option);
	extern LIBGCF1_API int gcf1_dbmng_keyword_download(HCTM handle, int option);

	extern LIBGCF1_API int gcf1_extra1_begin(const char *config_path, int option, HCTE *handle);
	extern LIBGCF1_API int gcf1_extra1_begin2(const char *config_path, int option,const char *lang, HCTE *handle);
	extern LIBGCF1_API int gcf1_extra1_end(HCTE handle, int option);
	extern LIBGCF1_API int gcf1_extra1_check_contents(HCTE handle, const char *contents, int option , struct category_result_ex1_s *category_result);
	extern LIBGCF1_API int gcf1_extra1_get_capability(int option , unsigned char *capability);

	extern LIBGCF1_API const char * gcf1_error_string (int error_code);

	extern LIBGCF1_API int gcf1_init (const char *config_path, const char *prefix);
	extern LIBGCF1_API int gcf1_fini ();

	extern LIBGCF1_API int gcf1_trace_log_rotate ();
	extern LIBGCF1_API int gcf1_trace_log_set_level (int level);

	extern LIBGCF1_API int gcf1_check_list_cat_versionset(HCTC handle, int option, struct list_result_s **result);
	extern LIBGCF1_API int gcf1_check_free_list_cat_versionset(HCTC handle, struct list_result_s *result);

	extern LIBGCF1_API int gcf1_reserve_cat_versionset(const char* version);

	extern LIBGCF1_API int gcf1_categorize(HCTC handle, const char *url_or_ip, int option1, int option2, struct categorize_result_s *categorize_result);
	extern LIBGCF1_API int gcf1_categorize_clear_cachedb(HCTC handle, int option, const char *domain);
	extern LIBGCF1_API int gcf1_get_info_list(HCTC handle, int option, int *list, int *list_length);
	extern LIBGCF1_API int gcf1_get_info_detail(HCTC handle, int option, int id, struct category_info_s *info);


#define GCF1_OPT_NONE   0x0000
#define GCF1_OPT_DL_FORCE_FULLDOWNLOAD  0x0100
#define GCF1_OPT_CHECK_EMBEDDED_URL 0x00000001

#define GCF1_CHECK2_FLAG_ENABLE   0x01
#define GCF1_CHECK2_FLAG_CHECKED  0x02
#define GCF1_CHECK2_FLAG_HASMORE  0x04
#define GCF1_CHECK2_FLAG_HASLEFT  0x08
#define GCF1_CHECK2_FLAG_HASRIGHT 0x10
#define GCF1_CHECK2_FLAG_GENERIC  0x20

#define GCF1_OPT_CHECK3_IAB_DEFAULT_MATCHING      0x10000000
#define GCF1_OPT_CHECK3_IAB_LONGEST_MATCHING      0x10000100
#define GCF1_OPT_CHECK3_IAB_CASCADE_MATCHING      0x10000200
#define GCF1_OPT_CHECK3_IAB_SIMPLE_MATCHING       0x10000400
#define GCF1_OPT_CHECK3_IAB_DOMAINONLY_MATCHING   0x10000800

#define GCF1_OPT_DBMNG_UPDATE_DEFAULT 0x0000000F
#define GCF1_OPT_DBMNG_UPDATE_NSDB    0x00000001
#define GCF1_OPT_DBMNG_UPDATE_USRDB   0x00000002
#define GCF1_OPT_DBMNG_UPDATE_KWDB    0x00000004
#define GCF1_OPT_DBMNG_UPDATE_SMALLDB 0x00000008
#define GCF1_OPT_DBMNG_CLEAR_CACHEDB  0x00000010
#define GCF1_OPT_DBMNG_UPDATE_IPDB    0x00001000
#define GCF1_OPT_DBMNG_CLEAR_IPDB_CACHEDB    0x00010000

#define GCF1_IAB_CATEGORY_IQG_CONTENT_RATING 0x00010000
#define GCF1_IAB_CATEGORY_IQG_NON_STANDARD 0x00020000
#define GCF1_IAB_CATEGORY_IQG_ILLEGAL_CONTENT 0x00030000
#define GCF1_IAB_CATEGORY_IQG_BRAND_SAFE 0x00040000

#define GCF1_OPT_LIST_VERSET_WITHOUT_HEADER 0x0000
#define GCF1_OPT_LIST_VERSET_WITH_HEADER 0x0001

#define GCF1_OPT_DBMNG_RELOAD_STANDBY 0x00010001
#define GCF1_OPT_DBMNG_RELOAD_SWAP 0x00010002
#define GCF1_OPT_DBMNG_RELOAD_RELEASE 0x00010004

#define GCF1_CATEGORIZE_FLAG_ENABLE        0x0100
#define GCF1_CATEGORIZE_FLAG_EMBEDDED_URL  0x0200
#define GCF1_CATEGORIZE_FLAG_USRDB         0x0400
#define GCF1_CATEGORIZE_FLAG_HASREPUTATION 0x0800
#define GCF1_CATEGORIZE_FLAG_HASAGERATING  0x1000
#define GCF1_CATEGORIZE_FLAG_KWDB          0x0001
#define GCF1_CATEGORIZE_FLAG_CACHEDB       0x0002
#define GCF1_CATEGORIZE_FLAG_LOCALDB       0x0004
#define GCF1_CATEGORIZE_FLAG_CLOUDDB       0x0008
#define GCF1_CATEGORIZE_FLAG_SMALLDB       0x0010

#define GCF1_CATEGORIZE_FLAG_URLDB         0x00010000
#define GCF1_CATEGORIZE_FLAG_IPDB          0x00020000

#define GCF1_OPT_CATEGORIZE_EMBEDDED_URL                  0x00000001
#define GCF1_OPT_CATEGORIZE_DEFAULT_MATCHING              0x00000000
#define GCF1_OPT_CATEGORIZE_LONGEST_MATCHING              0x00000100
#define GCF1_OPT_CATEGORIZE_CASCADE_MATCHING              0x00000200
#define GCF1_OPT_CATEGORIZE_SIMPLE_MATCHING               0x00000400
#define GCF1_OPT_CATEGORIZE_DOMAINONLY_MATCHING           0x00000800
#define GCF1_OPT_CATEGORIZE_HYBRID_DEFAULT_MATCHING       0x00001000
#define GCF1_OPT_CATEGORIZE_HYBRID_LONGEST_MATCHING       0x00001100
#define GCF1_OPT_CATEGORIZE_HYBRID_DEFAULT_LOCAL_MATCHING 0x00001003
#define GCF1_OPT_CATEGORIZE_HYBRID_DEFAULT_CLOUD_MATCHING 0x00001004
#define GCF1_OPT_CATEGORIZE_HYBRID_LONGEST_LOCAL_MATCHING 0x00001103
#define GCF1_OPT_CATEGORIZE_HYBRID_LONGEST_CLOUD_MATCHING 0x00001104

#define GCF1_OPT_CATEGORIZE_URLDB_ONLY 0x00010000
#define GCF1_OPT_CATEGORIZE_IPDB_ONLY  0x00020000

#define GCF1_OPT_GET_CATEGORY       0x00000
#define GCF1_OPT_GET_REPUTAION      0x00001
#define GCF1_OPT_GET_IAB_CATEGORY   0x00002
#define GCF1_OPT_GET_AGE_RATING     0x00003
#define GCF1_OPT_GET_CATEGORY_GROUP 0x00004

#ifdef __cplusplus
#ifndef WIN32
}
#endif
#endif

#include "gcf1_error.h"


#endif /* _GCF1_H_ */

