--TEST--
Test that sapi_windows_vt100_support exists only on Windows
--SKIPIF--
<?php
if(PHP_OS_FAMILY === "Windows") {
    echo "skip Only for not Windows systems";
}
?>
--FILE--
<?php

function fn340796939()
{
    var_dump(function_exists('sapi_windows_vt100_support'));
}
fn340796939();
--EXPECT--
bool(false)
