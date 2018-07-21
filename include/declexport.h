#ifndef EASYGO_DECL_EXPORT_H
#define EASYGO_DECL_EXPORT_H

// #pragma warning( disable: 4251 )

// 以easygo库为例，说明如何使用本头文件定义的宏。
//		
// 1. easygo为动态库，在easygo库的头文件定义以下宏。EXAMPLE_DECL用于导出声明，
//		而EXPORT_EASYGO则需要在easygo库的项目文件中定义。
//		
//		#ifndef EASYGO_DECL
//		#	ifdef EXPORT_EASYGO
//		#		define EASYGO_DECL COMTOP_DECL_EXPORT
//		#	else
//		#		define EASYGO_DECL COMTOP_DECL_IMPORT
//		#	endif
//		#endif
//		
// 2. easygo为静态库，在easygo库的头文件定义以下宏。EASYGO_DECL用于导出声明。
//		
//		#ifndef EASYGO_DECL
//		#	define EASYGO_DECL 
//		#endif
//		
// 3. easygo可导出静态库和共享库，在easygo库的头文件定义以下宏。EASYGO_DECL用于
//		导出声明，而EXPORT_EASYGO则需要在easygo库的项目文件中定义。如果easygo库
//		需要导出为动态库，则需要在项目文件定义EASYGO_DYN_LINK，且使用easygo库的工
//		程也需要定义EASYGO_DYN_LINK。这种方式其实结合了以上两种方式。
//		
//		#ifndef EASYGO_DECL
//		#	ifdef EASYGO_DYN_LINK
//		#		ifdef EXPORT_EASYGO
//		#			define EASYGO_DECL COMTOP_DECL_EXPORT
//		#		else
//		#			define EASYGO_DECL COMTOP_DECL_IMPORT
//		#		endif
//		#	else
//		#		define EASYGO_DECL 
//		#	endif
//		#endif
//		


#ifndef EASYGO_DECL_EXPORT
#  if defined(_MSC_VER)
#    define EASYGO_DECL_EXPORT __declspec(dllexport)
#  elif defined(__GNUC__)
#    define EASYGO_DECL_EXPORT __attribute__((visibility("default")))
#  else
#    error   "Unknown Operation System!"
#  endif
#endif

#ifndef EASYGO_DECL_IMPORT
#  if defined(_MSC_VER)
#    define EASYGO_DECL_IMPORT __declspec(dllimport)
#  else
#    define EASYGO_DECL_IMPORT
#  endif
#endif

#endif
