/*============================================================================
  CMake - Cross Platform Makefile Generator
  Copyright 2000-2009 Kitware, Inc., Insight Software Consortium

  Distributed under the OSI-approved BSD License (the "License");
  see accompanying file Copyright.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even the
  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the License for more information.
============================================================================*/
#ifndef cmQTWrapCPPCommand_h
#define cmQTWrapCPPCommand_h

#include "cmCommand.h"

#include "cmSourceFile.h"

/** \class cmQTWrapCPPCommand
 * \brief Create moc file rules for QT classes
 *
 * cmQTWrapCPPCommand is used to create wrappers for QT classes into
 * normal C++
 */
class cmQTWrapCPPCommand : public cmCommand
{
public:
  cmTypeMacro(cmQTWrapCPPCommand, cmCommand);

  /**
   * This is a virtual constructor for the command.
   */
  virtual cmCommand* Clone()
    {
    return new cmQTWrapCPPCommand;
    }

  /**
   * This is called when the command is first encountered in
   * the CMakeLists.txt file.
   */
  virtual bool InitialPass(std::vector<std::string> const& args,
                           cmExecutionStatus &status);

  /**
   * The name of the command as specified in CMakeList.txt.
   */
  virtual const char* GetName() { return "qt_wrap_cpp";}

  /**
   * Succinct documentation.
   */
  virtual const char* GetTerseDocumentation()
    {
    return "Create Qt Wrappers.";
    }

  /**
   * More documentation.
   */
  virtual const char* GetFullDocumentation()
    {
    return
      "  qt_wrap_cpp(resultingLibraryName DestName\n"
      "              SourceLists ...)\n"
      "Produce moc files for all the .h files listed in the SourceLists.  "
      "The moc files will be added to the library using the DestName "
      "source list.";
    }
};



#endif
