--TEST--
032: Namespace support for user functions (php name)
--FILE--
<?php

class Test
{
    static function foo()
    {
        echo __CLASS__, "::", __FUNCTION__, "\n";
    }
}
function foo()
{
    echo __FUNCTION__, "\n";
}
function fn995578950()
{
    call_user_func(__NAMESPACE__ . "\\foo");
    call_user_func(__NAMESPACE__ . "\\test::foo");
}
fn995578950();
--EXPECT--
foo
Test::foo
