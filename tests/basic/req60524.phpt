--TEST--
Req #60524 (Specify temporary directory)
--INI--
sys_temp_dir=/path/to/temp/dir
--SKIPIF--
<?php
if(PHP_OS_FAMILY === "Windows") {
    die('skip non-windows only test');
}
?>
--FILE--
<?php

function fn1665683846()
{
    echo sys_get_temp_dir();
}
fn1665683846();
--EXPECT--
/path/to/temp/dir
