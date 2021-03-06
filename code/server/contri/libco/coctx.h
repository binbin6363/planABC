/*
* Tencent is pleased to support the open source community by making Libco available.

* Copyright (C) 2014 THL A29 Limited, a Tencent company. All rights reserved.
*
* Licensed under the Apache License, Version 2.0 (the "License"); 
* you may not use this file except in compliance with the License. 
* You may obtain a copy of the License at
*
*	http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, 
* software distributed under the License is distributed on an "AS IS" BASIS, 
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
* See the License for the specific language governing permissions and 
* limitations under the License.
*/

// 此版本是经过优化的，大大减小占用的内存，bbwang


#ifndef __CO_CTX_H__
#define __CO_CTX_H__
#include <stdlib.h>
typedef int (*coctx_pfn_t)( const char *s,const char *s2 );
struct coctx_param_t
{
	coctx_pfn_t f;
	coctx_pfn_t f_link;
	const void *s1;
	const void *s2;
};
struct coctx_t
{
	void *regs[ 5 ];

	coctx_param_t *param;

	coctx_pfn_t routine;
	const void *s1;
	const void *s2;
	size_t ss_size;
	char *ss_sp;
	
};
int coctx_init( coctx_t *ctx );
int coctx_make( coctx_t *ctx,coctx_pfn_t pfn,const void *s,const void *s1 );
#endif
