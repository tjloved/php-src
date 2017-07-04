--TEST--
Closures can be unused generators
--FILE--
<?php

function fn1680645106()
{
    (function () {
        yield;
    })();
    echo "ok\n";
}
fn1680645106();
--EXPECT--
ok