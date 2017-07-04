--TEST--
Class public constant visibility
--FILE--
<?php

class A
{
    public const publicConst = 'publicConst';
    static function staticConstDump()
    {
        var_dump(self::publicConst);
    }
    function constDump()
    {
        var_dump(self::publicConst);
    }
}
function fn1493260715()
{
    var_dump(A::publicConst);
    A::staticConstDump();
    (new A())->constDump();
}
fn1493260715();
--EXPECTF--
string(11) "publicConst"
string(11) "publicConst"
string(11) "publicConst"
