--TEST--
Test for buffering in core functions with implicit flush on
--INI--
implicit_flush=1
--FILE--
<?php

function fn738468658()
{
    $res = var_export("foo1");
    echo "\n";
    $res = var_export("foo2", TRUE);
    echo "\n";
    echo $res . "\n";
}
fn738468658();
--EXPECT--
'foo1'

'foo2'
