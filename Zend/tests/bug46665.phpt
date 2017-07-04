--TEST--
Bug #46665 (Triggering autoload with a variable classname causes truncated autoload param)
--FILE--
<?php

function fn1078719122()
{
    spl_autoload_register(function ($class) {
        var_dump($class);
        require __DIR__ . '/bug46665_autoload.inc';
    });
    $baz = '\\Foo\\Bar\\Baz';
    new $baz();
}
fn1078719122();
--EXPECTF--
%string|unicode%(11) "Foo\Bar\Baz"
