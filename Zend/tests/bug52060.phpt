--TEST--
Bug #52060 (Memory leak when passing a closure to method_exists())
--FILE--
<?php

function fn756477941()
{
    $closure = function ($a) {
        echo $a;
    };
    var_dump(method_exists($closure, '__invoke'));
    // true

}
fn756477941();
--EXPECT--
bool(true)
