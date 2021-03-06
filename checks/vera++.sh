# Set up local profile vera++.profile with:
#    sudo ln -s /home/.../workspace/nuspell/checks/vera++.profile \
#    /usr/lib/vera++/profiles/vera++.profile
# See also https://bitbucket.org/verateam/vera/issues/60/allow-to-use-absolute-path-for-profile

cd ..

# Header file config.h is omitted.

vera++ \
-s -p vera++.profile -P max-line-length=80 src/nuspell/*.[ch]xx \
-o checks/vera++.txt

# vera++ has issue with string unit test file.
#vera++ \
#-s -p vera++.profile -P max-line-length=80 src/nuspell/*.[ch]xx tests/*.[ch]xx \
#-o checks/vera++.txt

#vera++ \
#-s -p vera++.profile -P max-line-length=80 src/nuspell/*.[ch]xx tests/*.[ch]xx \
#-x checks/vera++.xml

cd checks
