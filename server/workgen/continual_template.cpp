#include <stdlib.h>
#include <stdio.h>

/*
<file_info>
    <name><OUTFILE_1/>.zip</name>
    <generated_locally/>
    <upload_when_present/>
    <max_nbytes>50000000</max_nbytes>
    <url>http://quakecatcher.net/sensor_cgi/file_upload_handler</url>
</file_info>
<result>
    <file_ref>
        <file_name><OUTFILE_1/>.zip </file_name>
        <open_name>qcnout1.zip</open_name>
        <optional>1</optional>
        <no_validate>1</no_validate>
    </file_ref>
</result>
*/

#define MAX_UPLOAD_FILES 50

int main(int argc, char** argv)
{
   FILE* fOut = fopen("templates/qcn_output_continual.xml", "w");
   for (int i = 1; i <= MAX_UPLOAD_FILES; i++) {
       fprintf(fOut, "<file_info>\n"
            "   <name><OUTFILE_%d/>.zip</name>\n"
            "   <generated_locally/>\n"
            "   <upload_when_present/>\n"
            "   <max_nbytes>50000000</max_nbytes>\n"
            "   <url>http://quakecatcher.net/sensor_cgi/file_upload_handler</url>\n</file_info>\n", i
        );
   }
   fprintf(fOut, "<result>\n");
   for (int i = 1; i <= MAX_UPLOAD_FILES; i++) {
       fprintf(fOut, "  <file_ref>\n"
                     "     <file_name><OUTFILE_%d/>.zip</file_name>\n"
                     "     <open_name>qcnout%d.zip</open_name>\n"
                     "     <optional>1</optional>\n"
                     "     <no_validate>1</no_validate>\n"
                     "  </file_ref>\n", 
              i, i
       );
   }
   fprintf(fOut, "</result>\n");
   fclose(fOut);
}
