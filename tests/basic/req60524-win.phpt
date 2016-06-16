--TEST--
Req #60524 (Specify temporary directory)
--INI--
sys_temp_dir=C:\Windows
--SKIPIF--
<?php
if( substr(PHP_OS, 0, 3) != "WIN" )
  die('skip Run only on Windows');
?>
--FILE--
<?php

function fn708902138()
{
    echo sys_get_temp_dir();
}
fn708902138();
--EXPECT--
C:\\Windows
