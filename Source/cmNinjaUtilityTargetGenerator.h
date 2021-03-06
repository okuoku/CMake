/*============================================================================
  CMake - Cross Platform Makefile Generator
  Copyright 2011 Nicolas Despres <nicolas.despres@gmail.com>

  Distributed under the OSI-approved BSD License (the "License");
  see accompanying file Copyright.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even the
  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the License for more information.
============================================================================*/
#ifndef cmNinjaUtilityTargetGenerator_h
#  define cmNinjaUtilityTargetGenerator_h

#  include "cmNinjaTargetGenerator.h"
#  include "cmNinjaTypes.h"

class cmSourceFile;

class cmNinjaUtilityTargetGenerator : public cmNinjaTargetGenerator
{
public:
  cmNinjaUtilityTargetGenerator(cmTarget* target);
  ~cmNinjaUtilityTargetGenerator();

  void Generate();
};

#endif // ! cmNinjaUtilityTargetGenerator_h
