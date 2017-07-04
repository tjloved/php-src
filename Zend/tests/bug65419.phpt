--TEST--
Bug #65419 (Inside trait, self::class != __CLASS__)
--FILE--
<?php

trait abc
{
    static function def()
    {
        echo self::class, "\n";
        echo __CLASS__, "\n";
    }
}
class ghi
{
    use abc;
}
function fn62460232()
{
    ghi::def();
}
fn62460232();
--EXPECTF--
ghi
ghi