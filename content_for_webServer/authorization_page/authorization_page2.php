<?php
    print_r("Status: 200 OK\r\n");
    print_r("Content-type:text/html\r\n\r\n");
    $str = getenv('QUERY_STRING=');
    $strEncode = base64_encode($str);
    $strEncode .= "\n";
    $handle = fopen("./content_for_webServer/authorization_page/password.txt", "r");
    if ($handle) {
        while (($buffer = fgets($handle, 4096)) !== false) {
            if ($strEncode == $buffer)
            {
                echo "Hello!!!!\r\n";
                fclose($handle);
                exit;
            }
        }
        echo "User not found!!!!\r\n";
        fclose($handle);
    }
    else
    {
        echo "No registered users!\r\n";
    }
?>