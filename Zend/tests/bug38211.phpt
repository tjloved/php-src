--TEST--
Bug #38211 (variable name and cookie name match breaks script execution)
--FILE--
<?php

function fn1095952914()
{
    $test = 'test';
    unset(${$test});
    echo "ok\n";
}
fn1095952914();
--EXPECT--
ok
