--TEST--
Testing dynamic calls using variable variables with curly syntax
--FILE--
<?php

function fn433946334()
{
    $a = 'b';
    $b = 'c';
    $c = 'strtoupper';
    var_dump(${${$a}}('foo') == 'FOO');
    $a = 'b';
    $b = 'c';
    $c = 'strtoupper';
    $strtoupper = 'strtolower';
    var_dump(${${++$a}}('FOO') == 'foo');
}
fn433946334();
--EXPECT--
bool(true)
bool(true)
