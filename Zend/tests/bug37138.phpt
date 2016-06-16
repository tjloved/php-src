--TEST--
Bug #37138 (__autoload tries to load callback'ed self and parent)
--FILE--
<?php

function __autoload($CN)
{
    var_dump($CN);
}
class st
{
    public static function e()
    {
        echo "EHLO\n";
    }
    public static function e2()
    {
        call_user_func(array('self', 'e'));
    }
}
class stch extends st
{
    public static function g()
    {
        call_user_func(array('parent', 'e'));
    }
}
function fn1390333563()
{
    st::e();
    st::e2();
    stch::g();
}
fn1390333563();
--EXPECT--
EHLO
EHLO
EHLO

