COMMENT =	LLVM OpenMP runtime library

V =		22.1.6
DISTNAME =	llvm-project-${V}.src
PKGNAME =	libomp-${V}
CATEGORIES =	devel

HOMEPAGE =	https://openmp.llvm.org/
MAINTAINER =	Frank Baster <frank@baster.nz>

# Apache-2.0 with LLVM exceptions
PERMIT_PACKAGE =	Yes

SITES =		https://github.com/llvm/llvm-project/releases/download/llvmorg-${V}/
EXTRACT_SUFX =	.tar.xz

SHARED_LIBS +=	omp	0.0
MODULES =	devel/cmake
WRKSRC =	${WRKDIST}/runtimes

CONFIGURE_ARGS =	-DLLVM_ENABLE_RUNTIMES=openmp \
			-DLIBOMP_INSTALL_ALIASES=OFF \
			-DLIBOMP_OMPT_SUPPORT=OFF \
			-DLIBOMP_USE_HWLOC=OFF \
			-DOPENBSD_LIBOMP_VERSION=${LIBomp_VERSION} \
			-DOPENMP_ENABLE_LIBOMPTARGET=OFF \
			-DCMAKE_BUILD_TYPE=Release

ONLY_FOR_ARCHS =	amd64

WANTLIB =	m pthread

do-test:
	cd ${WRKBUILD} && \
	${CXX} -fopenmp -I${WRKINST}${LOCALBASE}/include \
	    -L${WRKINST}${LOCALBASE}/lib \
	    -Wl,-rpath,${WRKINST}${LOCALBASE}/lib \
	    ${FILESDIR}/omptest.cpp -o omptest && \
	./omptest

.include <bsd.port.mk>
