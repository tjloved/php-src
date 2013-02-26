--TEST--
Test writing to a new expression
--FILE--
<?php

class Foo {
    public function __set($name, $value) {
        echo "Setting $name to $value";
    }
}

(new Foo)->foo = 'bar';

?>
--EXPECT--
Setting foo to bar
