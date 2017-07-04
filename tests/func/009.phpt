--TEST--
Test for buffering in core functions with implicit flush on
--INI--
implicit_flush=1
--FILE--
<?php

function fn2124006110()
{
    $res = var_export("foo1");
    echo "\n";
    $res = var_export("foo2", TRUE);
    echo "\n";
    echo $res . "\n";
}
fn2124006110();
--EXPECT--
'foo1'

'foo2'
