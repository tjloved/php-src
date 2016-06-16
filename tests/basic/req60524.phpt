--TEST--
Req #60524 (Specify temporary directory)
--INI--
sys_temp_dir=/path/to/temp/dir
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip non-windows only test');
}
?>
--FILE--
<?php

function fn904088461()
{
    echo sys_get_temp_dir();
}
fn904088461();
--EXPECT--
/path/to/temp/dir
