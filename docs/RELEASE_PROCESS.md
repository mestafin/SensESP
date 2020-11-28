# SensESP release process

This release process is loosely based on the git-flow model with a few
simplifications. Main differences are:

- Since releasing is a rapid process, long-lived release branches are not
  needed. Releases can be made directly from feature branches.
- `master` (or `main` in the future) is the unstable development branch.
- `latest` is the last released version. Releases must be tagged from
  this branch.

The release process is as follows:

1. Create a new local feature branch for the release.

2. Bump the version number in `Doxyfile`, `library.properties` and 
   `library.json`.

   Commit the changes.

2. Update autogenerated html and js source files:

    pio run -e d1_mini -t webUI

   Remember to commit the results.

3. Update the doxygen docs (first bump the version number in `Doxyfile`)

    rm -rf docs/generated/docs/
    doxygen

   Again, commit the results.

4. Push the changes to your local fork.

5. Verify that everything looks OK in your fork.

   Any release testing should happen now at this point.

6. Once everything is fine, create a PR against the `latest` branch.
   
   Merge it.

7. Navigate to https://github.com/SignalK/SensESP/releases and click on 
   "Draft a new release". Tag version is "x.y.z". Be sure to select `latest`
   branch. Title is ordinarily "Version x.y.z". Description would ideally
   be a listing of new features and bugfixes.

8. The release is now done. Finally, create a PR from `latest` to `master`
   and merge it. This will bring `master` up to date with the released version.