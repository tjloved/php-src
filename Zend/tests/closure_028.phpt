--TEST--
Closure 028: Trying to use lambda directly in foreach
--FILE--
<?php

function fn452343620()
{
    foreach (function () {
        return 1;
    } as $y) {
        var_dump($y);
    }
    print "ok\n";
}
fn452343620();
--EXPECT--
ok
