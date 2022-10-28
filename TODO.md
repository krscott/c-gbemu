# TODO

- [ ] Clean up RAM/ROM struct
  - Allow array to be stack/static-allocated
  - Replace with generic vector type?
- [ ] Standardize deinit
  - Add missing null checks to deinit functions
  - Early return if null
  - Update docstrings to reflect null is ok
  - Should deinit() still be required if init() errored? If so, remove deinit() call from init()?
- [ ] Standardize bus registers
  - Union: array access + field access?
- [ ] Make UI state a struct
- [ ] video_buffer fixed-size struct