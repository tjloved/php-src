--TEST--
Bug #38211 (variable name and cookie name match breaks script execution)
--FILE--
<?php

function fn30838536()
{
    $test = 'test';
    unset(${$test});
    echo "ok\n";
}
fn30838536();
--EXPECT--
ok
