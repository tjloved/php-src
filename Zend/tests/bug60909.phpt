--TEST--
Bug #60909: custom error handler throwing Exception + fatal error = no shutdown function
--FILE--
<?php

register_shutdown_function(function() {
    echo "!!!shutdown!!!";
});
set_error_handler(function() {
    throw new Exception("Foo");
});

require 'notfound.php';
?>
--EXPECTF--
Fatal error: main(): Failed opening required 'notfound.php' (include_path=%s) in %s on line %d
!!!shutdown!!!
