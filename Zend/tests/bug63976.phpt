--TEST--
Bug #63976 (Parent class incorrectly using child constant in class property)
--FILE--
<?php

function fn446208508()
{
    if (1) {
        class Foo
        {
            const TABLE = "foo";
            public $table = self::TABLE;
        }
    }
    if (1) {
        class Bar extends Foo
        {
            const TABLE = "bar";
        }
    }
    $bar = new Bar();
    var_dump($bar->table);
}
fn446208508();
--EXPECT--
string(3) "foo"
