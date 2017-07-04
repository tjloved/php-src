--TEST--
Bug #42798 (_autoload() not triggered for classes used in method signature)
--FILE--
<?php

function foo($c = ok::constant)
{
}
function fn269044375()
{
    spl_autoload_register(function ($className) {
        print "{$className}\n";
        exit;
    });
    foo();
}
fn269044375();
--EXPECT--
ok
