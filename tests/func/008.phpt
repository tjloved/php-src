--TEST--
Test for buffering in core functions with implicit flush off
--INI--
implicit_flush=0
--FILE--
<?php

function fn918193806()
{
    $res = var_export("foo1");
    echo "\n";
    $res = var_export("foo2", TRUE);
    echo "\n";
    echo $res . "\n";
}
fn918193806();
--EXPECT--
'foo1'

'foo2'
