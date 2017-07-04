--TEST--
Bug #36568 (memory_limit has no effect)
--SKIPIF--
<?php 
	if (!function_exists('memory_get_usage')) die('skip PHP is configured without memory_limit');
?>
--INI--
memory_limit=16M
--FILE--
<?php

function fn747600061()
{
    ini_set("memory_limit", "32M");
    echo ini_get("memory_limit");
}
fn747600061();
--EXPECT--
32M
