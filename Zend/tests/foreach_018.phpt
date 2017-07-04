--TEST--
Foreach on stdClass with properties looking like mangled properties
--FILE--
<?php

function fn2135724457()
{
    $obj = (object) ["\0A\0b" => 42, "\0*\0c" => 24];
    foreach ($obj as $k => $v) {
        var_dump($k, $v);
    }
}
fn2135724457();
--EXPECT--
string(1) "b"
int(42)
string(1) "c"
int(24)
