--TEST--
Bug #53347 Segfault accessing static method
--FILE--
<?php

class ezcConsoleOutput
{
    protected static $color = array('gray' => 30);
    public static function isValidFormatCode($type, $key)
    {
        return isset(self::${$type}[$key]);
    }
}
function fn1820241117()
{
    var_dump(ezcConsoleOutput::isValidFormatCode('color', 'gray'));
}
fn1820241117();
--EXPECT--
bool(true)
