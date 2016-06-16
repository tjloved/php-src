--TEST--
Return value separation

--FILE--
<?php

function test1(&$abc) : string
{
    return $abc;
}
function &test2(int $abc) : string
{
    return $abc;
}
function &test3(int &$abc) : string
{
    return $abc;
}
function fn619258233()
{
    $a = 123;
    var_dump(test1($a));
    var_dump($a);
    var_dump(test2($a));
    var_dump($a);
    var_dump(test3($a));
    var_dump($a);
}
fn619258233();
--EXPECTF--
string(3) "123"
int(123)
string(3) "123"
int(123)
string(3) "123"
string(3) "123"