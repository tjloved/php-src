--TEST--
067: Name ambiguity (import name & internal class name)
--FILE--
<?php

function fn710297734()
{
    include __DIR__ . '/ns_022.inc';
    include __DIR__ . '/ns_027.inc';
    include __DIR__ . '/ns_067.inc';
}
fn710297734();
--EXPECT--
Foo\Bar\Foo
