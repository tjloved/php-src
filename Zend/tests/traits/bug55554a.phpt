--TEST--
Bug #55137 (Legacy constructor not registered for class)
--FILE--
<?php

// All constructors should be registered as such
trait TConstructor
{
    public function constructor()
    {
        echo "ctor executed\n";
    }
}
class NewConstructor
{
    use TConstructor {
        constructor as __construct;
    }
}
class LegacyConstructor
{
    use TConstructor {
        constructor as LegacyConstructor;
    }
}
function fn583364512()
{
    echo "New constructor: ";
    $o = new NewConstructor();
    echo "Legacy constructor: ";
    $o = new LegacyConstructor();
}
fn583364512();
--EXPECT--
New constructor: ctor executed
Legacy constructor: ctor executed
