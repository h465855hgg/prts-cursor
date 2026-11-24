%global debug_package %{nil}

Name:           prts-cursor
Version:        1.0.0
Release:        1%{?dist}
Summary:        PRTS cursor theme and shader effect switcher for Umbriel
License:        MIT
URL:            https://github.com/h465855hgg/prts-cursor
BuildArch:      %{?buildarch}
Requires:       gtk3
Requires:       python3
Requires:       hicolor-icon-theme

%description
A small GUI to switch the Xcursor theme, cursor size and Umbriel cursor
shader effects. Built with HuxerUI. Ships a tray icon and applies changes
immediately by writing ~/.config/umbriel/config.toml and reloading Umbriel.

%install
install -d %{buildroot}/opt/prts-cursor
cp -a %{stage}/opt/prts-cursor/. %{buildroot}/opt/prts-cursor/
install -d %{buildroot}%{_bindir}
ln -s ../opt/prts-cursor/prts-cursor %{buildroot}%{_bindir}/prts-cursor
install -d %{buildroot}%{_datadir}/applications
install -m644 %{stage}/usr/share/applications/prts-cursor.desktop %{buildroot}%{_datadir}/applications/
install -d %{buildroot}%{_datadir}/icons/hicolor/128x128/apps
install -m644 %{stage}/usr/share/icons/hicolor/128x128/apps/prts-cursor.png %{buildroot}%{_datadir}/icons/hicolor/128x128/apps/
install -d %{buildroot}%{_datadir}/metainfo
install -m644 %{stage}/usr/share/metainfo/prts-cursor.metainfo.xml %{buildroot}%{_datadir}/metainfo/

%post
update-desktop-database -q %{_datadir}/applications >/dev/null 2>&1 || :
gtk-update-icon-cache -q -t -f %{_datadir}/icons/hicolor >/dev/null 2>&1 || :

%postun
update-desktop-database -q %{_datadir}/applications >/dev/null 2>&1 || :
gtk-update-icon-cache -q -t -f %{_datadir}/icons/hicolor >/dev/null 2>&1 || :

%files
/opt/prts-cursor
%{_bindir}/prts-cursor
%{_datadir}/applications/prts-cursor.desktop
%{_datadir}/icons/hicolor/128x128/apps/prts-cursor.png
%{_datadir}/metainfo/prts-cursor.metainfo.xml

%changelog
* Tue Nov 24 2026 h465855hgg <h465855hgg@users.noreply.github.com> - 1.0.0-1
- Initial package
