--TEST--
Req #60524 (Specify temporary directory)
--INI--
sys_temp_dir=C:\Windows
--SKIPIF--
<?php
if(PHP_OS_FAMILY !== "Windows")
  die('skip Run only on Windows');
?>
--FILE--
<?php

function fn1504164198()
{
    echo sys_get_temp_dir();
}
fn1504164198();
--EXPECT--
C:\\Windows
