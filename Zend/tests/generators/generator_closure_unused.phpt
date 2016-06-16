--TEST--
Closures can be unused generators
--FILE--
<?php

function fn1601843837()
{
    (function () {
        yield;
    })();
    echo "ok\n";
}
fn1601843837();
--EXPECT--
ok