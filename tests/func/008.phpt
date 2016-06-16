--TEST--
Test for buffering in core functions with implicit flush off
--INI--
implicit_flush=0
--FILE--
<?php

function fn1931419952()
{
    $res = var_export("foo1");
    echo "\n";
    $res = var_export("foo2", TRUE);
    echo "\n";
    echo $res . "\n";
}
fn1931419952();
--EXPECT--
'foo1'

'foo2'
