
Here is windows specific files.

In order to install *.inf file do following
   
   >pnputil -i -a inf_file_name.inf
 
 If command succed As a result of this command one should see 
 following output
    Processing inf :            OOI_HR2000Plus.inf
    Successfully installed the driver on a device on the system.
    Driver package added successfully.
    Published name :            out_inf_file_name.inf
 
 
In order to uninstall driver installed with above described step. Do following call
   
   >pnputil -d out_inf_file_name.inf
   >pnputil -f -d out_inf_file_name.inf    :: force delete



Following are the types and the files in each group

 1. inf files
    a) OOI_HR2000Plus2.inf  -> This file is for OceanOptics HR2000+ device
