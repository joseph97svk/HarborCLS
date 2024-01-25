function redirectToFigure(form) {
    let figure = form.value;

    if (figure === "Elegir") {
        return;
    }

    location.href = '/legoFigure?figure=' + figure;
}
