procedure Main_Ada is
   procedure Main_Cpp;
   pragma Import (C, Main_Cpp, "main_cpp");
begin
   Main_Cpp;
end Main_Ada;
