--TEST--
output buffering - multiple
--FILE--
<?php

function fn173630569()
{
    echo 0;
    ob_start();
    ob_start();
    ob_start();
    ob_start();
    echo 1;
    ob_end_flush();
    echo 2;
    $ob = ob_get_clean();
    echo 3;
    ob_flush();
    ob_end_clean();
    echo 4;
    ob_end_flush();
    echo $ob;
}
fn173630569();
--EXPECT--
03412
